import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import tkinter as tk
from tkinter import filedialog, messagebox
import math

# 比較したい統計指標をここで定義
#TARGET_METRICS = ["Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]
TARGET_METRICS = ["Debut_Score"]

def select_folders():
    """
    ユーザーにフォルダを選択させるGUIを表示します。
    キャンセルされるまで複数のフォルダを追加できます。
    """
    root = tk.Tk()
    root.withdraw()  # メインウィンドウを隠す
    
    folder_paths = []
    
    while True:
        if not folder_paths:
            title = "フォルダを選択してください"
        else:
            title = "追加のフォルダを選択しますか？（キャンセルで終了）"
            
        folder_selected = filedialog.askdirectory(title=title)
        
        if folder_selected:
            folder_paths.append(folder_selected)
            # 続けて選択するか確認
            if not messagebox.askyesno("継続確認", "他にも比較したいフォルダはありますか？"):
                break
        else:
            break
            
    return folder_paths

def load_csv_data(file_path):
    """
    CSVファイルを読み込み、数値データとメタデータを返します。
    """
    try:
        # 1. 数値データの読み込み
        # メタデータ行（列数が異なる）でのエラーを防ぐため、不正な行をスキップ
        try:
            df = pd.read_csv(file_path, index_col=0, on_bad_lines='skip')
        except TypeError:
            # pandasのバージョンが古い場合の互換性
            df = pd.read_csv(file_path, index_col=0, error_bad_lines=False)
        
        # データクリーニング: 'Average'行より下のメタデータ（training, val等）を削除する
        # インデックスに 'Average' がある場合、そこまでをデータとする
        if 'Average' in df.index:
            # locを使ってAverage行までを取得
            # 注意: インデックスが重複していない前提
            end_idx = df.index.get_loc('Average')
            # get_locがintを返す場合とsliceを返す場合があるため対応
            if isinstance(end_idx, int):
                df = df.iloc[:end_idx+1]
            elif isinstance(end_idx, slice):
                df = df.iloc[end_idx] # 重複がある場合はスライス
            
            # それでも念のため、インデックス名でフィルタリング（数値変換できない行を除外）
            # 今回のデータ形式なら 'Average' 以降の行を削除するロジックで十分
        
        #print(df)
        #exit()

        # 数値以外のデータが含まれている可能性を考慮して変換
        df = df.apply(pd.to_numeric, errors='coerce')
        
        # NaNを含む行（変換できなかったメタデータ行など）を削除
        df = df.dropna()
        
        # 2. メタデータの読み込み（テキストとして解析）
        metadata = {
            "train_period": "N/A",
            "train_per": "N/A"
        }
        
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            
        for line in lines:
            parts = line.strip().split(',')
            key = parts[0]
            
            # 年代情報の抽出 (training行: training,2015,2016...)
            if key == 'training':
                years = [p for p in parts[1:] if p]
                if years:
                    # 先頭と末尾の年を取得して期間とする
                    metadata["train_period"] = f"{years[0]}-{years[-1]}"
            
            # テスト年代情報の抽出 (test行: test,2020...)
            if key == 'test':
                years = [p for p in parts[1:] if p]
                if years:
                    metadata["test_period"] = years[0]

            # パーセンテージ情報の抽出 (traing_data(%),70.5)
            if key == 'traing_data(%)':
                if len(parts) > 1:
                    try:
                        val = float(parts[1])
                        metadata["train_per"] = f"{val:.1f}%"
                    except ValueError:
                        pass

        return df, metadata
    except Exception as e:
        print(f"Error loading {file_path}: {e}")
        return None, {}

def visualize_time_series(df, metrics):
    """
    8つの指標を1つの図（サブプロット）にまとめ、年代（Period）ごとの推移を可視化します。
    """
    # 年代順にソートするための補助列作成
    # Period形式: "2008-2012" などを想定
    def get_start_year(p):
        try:
            return int(str(p).split('-')[0])
        except:
            return 9999

    df['StartYear'] = df['Period'].apply(get_start_year)
    df = df.sort_values('StartYear')

    # サブプロット設定
    num_metrics = len(metrics)
    cols = 2
    rows = math.ceil(num_metrics / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(16, 6 * rows), constrained_layout=True)
    axes = axes.flatten()
    
    # ユニークな実験リスト（Hueの順序固定用）
    experiments = sorted(df['Experiment'].unique())

    for i, metric in enumerate(metrics):
        ax = axes[i]
        
        # 対象指標のデータ抽出
        metric_df = df[df['Metric'] == metric]
        
        if metric_df.empty:
            ax.text(0.5, 0.5, "No Data", ha='center', va='center')
            continue
            
        # プロット作成
        # X軸: Period, Y軸: Score, 色: Experiment
        sns.pointplot(
            data=metric_df, 
            x='Period', 
            y='Score', 
            hue='Experiment', 
            hue_order=experiments,
            ax=ax,
            markers='o',
            scale=0.8,
            errwidth=1.5,
            capsize=0.1,
            dodge=0.1
        )
        
        ax.set_title(f"Metric: {metric}", fontsize=14, fontweight='bold')
        ax.set_ylabel("Balanced Accuracy")
        ax.set_xlabel("Period")
        ax.grid(True, linestyle='--', alpha=0.6)
        
        # Y軸の範囲設定
        y_min = metric_df['Score'].min()
        y_min = max(0, y_min - 0.05)
        ax.set_ylim(y_min, 1.05)
        
        # X軸ラベルの回転
        ax.tick_params(axis='x', rotation=45)
        
        # 凡例の設定
        ax.legend(fontsize='small', title='Experiment')

    # 余ったサブプロットを非表示
    for j in range(i + 1, len(axes)):
        axes[j].axis('off')

    plt.suptitle("Performance Trends by Period (8 Metrics)", fontsize=18)
    plt.show()

def main():
    # 1. フォルダ選択
    folders = select_folders()
    
    if not folders:
        print("フォルダが選択されませんでした。")
        return

    all_data_list = []
    # 2. 各フォルダ内のCSVを探して読み込む
    for folder in folders:
        # results_*.csv のパターンに一致するファイルを探す
        # コンテキストに基づき 'results_*.csv' を対象とします
        csv_files = glob.glob(os.path.join(folder, "results_*.csv"))
        
        if not csv_files:
            print(f"警告: フォルダ '{folder}' 内に 'results_*.csv' が見つかりませんでした。")
            continue

        for file_path in csv_files:
            print(f"Loading file: {file_path}")
            #dfとmetadataを取得
            df, metadata = load_csv_data(file_path)
            if df is not None and not df.empty:
                # プロット用にデータを整形
                df.index.name = 'Metric'
                df_reset = df.reset_index()
                
                # ターゲット指標のみ抽出
                df_reset = df_reset[df_reset['Metric'].isin(TARGET_METRICS)]
                
                if df_reset.empty:
                    continue

                # Long形式に変換 (Metric, Experiment, Score)
                melted = df_reset.melt(id_vars=['Metric'], var_name='Experiment', value_name='Score')
                
                # Period列を追加 (test_period または ファイル名から推測)
                period = metadata.get("test_period", "Unknown")
                if period == "Unknown":
                    try:
                        # ファイル名 results_2008_2012.csv から末尾の年を取得
                        basename = os.path.basename(file_path)
                        parts = basename.replace("results_", "").replace(".csv", "").split("_")
                        if len(parts) >= 2:
                            period = parts[-1]
                    except:
                        pass
                
                melted['Period'] = period
                all_data_list.append(melted)

    if not all_data_list:
        print("有効なデータが見つかりませんでした。")
        return

    # 全データを結合
    final_df = pd.concat(all_data_list, ignore_index=True)

    # 3. 可視化実行
    visualize_time_series(final_df, TARGET_METRICS)

if __name__ == "__main__":
    main()
