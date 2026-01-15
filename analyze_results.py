#縦軸を正解度、横軸を年代として、指標ごとに比べる
#指標_実験_年代ごとに値が一番高かったものを表示する

import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import tkinter as tk
from tkinter import filedialog, messagebox
import math

# 比較したい統計指標を定義
TARGET_METRICS = ["Average", "Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]
#TARGET_METRICS = ["Average", "Debut_Score", "Max", "Mean"]
#TARGET_METRICS = ["Std", "Length", "Sum", "Skewness"]
#TARGET_METRICS = ["Skewness", "Kurtosis"]
#TARGET_METRICS = ["Average", "Kurtosis"]

SPOTIFY_POPULARITY = True

def select_folders():
    root = tk.Tk()
    root.withdraw()
    folder_paths = []
    while True:
        title = "フォルダを選択" if not folder_paths else "比較対象のフォルダを追加（キャンセルで終了）"
        folder_selected = filedialog.askdirectory(title=title)
        if folder_selected:
            folder_paths.append(folder_selected)
            if not messagebox.askyesno("継続確認", "他にも比較したいフォルダはありますか？"):
                break
        else:
            break
    return folder_paths

def load_csv_data(file_path):
    try:
        try:
            df = pd.read_csv(file_path, index_col=0, on_bad_lines='skip')
        except TypeError:
            df = pd.read_csv(file_path, index_col=0, error_bad_lines=False)
        
        if 'Average' in df.index:
            end_idx = df.index.get_loc('Average')
            if isinstance(end_idx, int):
                df = df.iloc[:end_idx+1]
            elif isinstance(end_idx, slice):
                df = df.iloc[end_idx]
            
        df = df.apply(pd.to_numeric, errors='coerce')
        df = df.dropna()
        
        metadata = {"test_period": "Unknown"}
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            for line in lines:
                parts = line.strip().split(',')
                if parts[0] == 'test':
                    years = [p for p in parts[1:] if p]
                    if years:
                        metadata["test_period"] = years[0]
        return df, metadata
    except Exception as e:
        print(f"Error loading {file_path}: {e}")
        return None, {}

def visualize_time_series(df, metrics):
    """
    色(hue)で実験を区別し、線種(style)でフォルダ(Dataset)を区別してプロットします。
    """
    # 年代順ソート用
    def get_start_year(p):
        try:
            # ハイフンで区切られた数字の部分を探す
            parts = str(p).split('-')
            for part in reversed(parts):
                if part.isdigit() and len(part) == 4:
                    return int(part)
        except:
            return 9999

    df['StartYear'] = df['Period'].apply(get_start_year)
    df = df.sort_values('StartYear')

    ordered_periods = df['Period'].unique()
    df['Period'] = pd.Categorical(df['Period'], categories=ordered_periods, ordered=True)

    num_metrics = len(metrics)
    cols = 2
    rows = math.ceil(num_metrics / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(16, 6 * rows), constrained_layout=True)
    if num_metrics == 1:
        axes = [axes] # 1つの場合リスト化して扱いを統一
    else:
        axes = axes.flatten()
    
    # 描画設定
    experiments = sorted(df['Experiment'].unique())
    datasets = sorted(df['Dataset'].unique()) # フォルダ名のリスト

    for i, metric in enumerate(metrics):
        ax = axes[i]
        metric_df = df[df['Metric'] == metric]
        
        if metric_df.empty:
            ax.text(0.5, 0.5, "No Data", ha='center', va='center')
            continue
            
        # === 変更点: sns.lineplotを使用 ===
        # hue: 実験名（色を変える）
        # style: フォルダ名（線の種類を変える: 実線、点線など）
        # markers: Trueにすると、styleに合わせてマーカーの形（〇, ×, △など）も自動で変わります
        sns.lineplot(
            data=metric_df, 
            x='Period', 
            y='Score', 
            hue='Experiment', 
            style='Dataset',     # <--- ここが重要！フォルダごとに線種を変える
            hue_order=experiments,
            style_order=datasets,
            markers=True,        # マーカーを表示
            dashes=True,         # 点線を使用
            ax=ax,
            linewidth=2,
            markersize=8,
            sort=False
        )
        
        ax.set_title(f"Metric: {metric}", fontsize=14, fontweight='bold')
        ax.set_ylabel("Balanced Accuracy")
        ax.set_xlabel("Period")
        ax.grid(True, linestyle='--', alpha=0.6)
        
        y_min = metric_df['Score'].min()
        y_min = max(0, y_min - 0.05)
        ax.set_ylim(y_min, 1.05)
        ax.tick_params(axis='x', rotation=45)
        
        # 凡例の位置調整
        ax.legend(fontsize='small', title='Exp / Dataset', loc='upper left', bbox_to_anchor=(1, 1))

    # 余ったサブプロット非表示
    if num_metrics > 1:
        for j in range(i + 1, len(axes)):
            axes[j].axis('off')

    plt.suptitle("Performance Comparison Across Folders", fontsize=18)
    plt.show()


def print_best_performers(df):
    """
    各指標(Metric)・実験(Experiment)ごとに、最もスコアが高かった年代(Period)を出力する
    """
    print("\n" + "="*80)
    print(f"{'Best Performance by Metric and Experiment':^80}")
    print("="*80)
    
    if df.empty:
        print("No data available.")
        return

    try:
        # MetricとExperimentでグループ化し、Scoreが最大の行を取得
        # idxmaxは最大値を持つ最初のインデックスを返す
        idx = df.groupby(['Metric', 'Experiment'])['Score'].idxmax()
        best_performers = df.loc[idx]
        
        # 見やすくソート (Metric順 -> Experiment順)
        best_performers = best_performers.sort_values(by=['Metric', 'Experiment'])
        
        # 表示するカラムを選択
        cols_to_show = ['Metric', 'Experiment', 'Period', 'Score']
        if 'Dataset' in best_performers.columns:
            cols_to_show.append('Dataset')
            
        # データフレームを文字列として整形して表示
        print(best_performers[cols_to_show].to_string(index=False, float_format=lambda x: "{:.4f}".format(x)))
        
    except Exception as e:
        print(f"Error calculating best performers: {e}")
        
    print("="*80 + "\n")


def main():
    folders = select_folders()
    if not folders:
        return

    all_data_list = []
    
    for folder in folders:
        # フォルダ名を識別子として取得 (例: "Experiment_A")
        folder_name = os.path.basename(os.path.normpath(folder))
        
        csv_files = glob.glob(os.path.join(folder, "results_*.csv"))
        if not csv_files:
            continue

        for file_path in csv_files:
            df, metadata = load_csv_data(file_path)
            if df is not None and not df.empty:
                df.index.name = 'Metric'
                df_reset = df.reset_index()
                df_reset = df_reset[df_reset['Metric'].isin(TARGET_METRICS)]
                
                if df_reset.empty:
                    continue

                #横持ちを縦持ちへ
                melted = df_reset.melt(id_vars=['Metric'], var_name='Experiment', value_name='Score')
                
                # Periodの決定
                period = metadata.get("test_period", "Unknown")
                print(f"Loaded {file_path}, determined period: {period}")
                
                if period == "Unknown":
                    try:
                        basename = os.path.basename(file_path)
                        parts = basename.replace("results_", "").replace(".csv", "").split("_")
                        if len(parts) >= 2:
                            period = parts[-1]
                    except:
                        pass
                melted['Period'] = period
                
                # === 変更点: データセット名（フォルダ名）を追加 ===
                melted['Dataset'] = folder_name 
                
                all_data_list.append(melted)

    if not all_data_list:
        print("有効なデータが見つかりませんでした。")
        return

    combined_df = pd.concat(all_data_list, ignore_index=True)
    final_df = combined_df[combined_df["Experiment"] != "Exp4: Spotify_Popularity"] if SPOTIFY_POPULARITY else combined_df

    # 最高値を出力
    print_best_performers(final_df)

    visualize_time_series(final_df, TARGET_METRICS)

if __name__ == "__main__":
    main()



