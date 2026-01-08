import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import tkinter as tk
from tkinter import filedialog, messagebox

# 比較したい統計指標をここで定義
TARGET_METRICS = ["Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]

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

def visualize_single_folder(df, title):
    """
    単一フォルダ（単一ファイル）の場合の可視化。
    すべての統計指標（Kurtosis, Max, Mean...）をヒートマップで表示。
    """
    plt.figure(figsize=(10, 8))
    sns.heatmap(df, annot=True, fmt=".3f", cmap="YlGnBu", linewidths=.5)
    plt.title(f"Statistical Metrics Heatmap: {title}")
    plt.ylabel("Metrics")
    plt.xlabel("Experiments")
    plt.tight_layout()
    plt.show()

def visualize_comparison(folder_data, metrics):
    """
    複数フォルダの場合の可視化。
    指定された指標ごとに、フォルダごとの平均値を算出して比較グラフを作成します。
    """
    if not folder_data:
        return

    for metric in metrics:
        print(f"Processing metric: {metric}")
        plot_data = []

        for folder, df in folder_data.items():
            if metric in df.index:
                # その指標の行を取得 (Series: index=Experiment, values=Score)
                row = df.loc[metric]
                for exp, score in row.items():
                    plot_data.append({
                        'Folder': folder,
                        'Experiment': exp,
                        'Score': score
                    })
        
        if not plot_data:
            print(f"指標 '{metric}' のデータが見つかりませんでした。")
            continue

        # 比較用DataFrame作成
        comp_df = pd.DataFrame(plot_data)

        # グラフ描画
        plt.figure(figsize=(12, 6))
        # x軸を実験(Experiment)、色(hue)をフォルダにして比較
        ax = sns.barplot(data=comp_df, x='Experiment', y='Score', hue='Folder', palette='viridis')
        
        # 数値を棒グラフの上に表示
        for container in ax.containers:
            ax.bar_label(container, fmt='%.3f', padding=3, fontsize=9)
        
        plt.title(f"Comparison of {metric} Scores (Average per Folder)")
        
        # スケール調整
        max_score = comp_df['Score'].max()
        if max_score <= 1.0:
            plt.ylim(0, 1.1)
            
        plt.grid(axis='y', linestyle='--', alpha=0.7)
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0)
        plt.tight_layout()
        plt.show()

def main():
    # 1. フォルダ選択
    folders = select_folders()
    
    if not folders:
        print("フォルダが選択されませんでした。")
        return

    all_data = {}
    
    # 2. 各フォルダ内のCSVを探して読み込む
    for folder in folders:
        # results_*.csv のパターンに一致するファイルを探す
        # コンテキストに基づき 'results_*.csv' を対象とします
        csv_files = glob.glob(os.path.join(folder, "results_*.csv"))
        
        if not csv_files:
            print(f"警告: フォルダ '{folder}' 内に 'results_*.csv' が見つかりませんでした。")
            continue
            
        folder_dfs = []
        for file_path in csv_files:
            df, _ = load_csv_data(file_path)
            if df is not None and not df.empty:
                folder_dfs.append(df)
        
        if folder_dfs:
            # フォルダ内の全データを結合し、平均を算出する
            # 同じ構造のDataFrameリストを結合
            combined_df = pd.concat(folder_dfs)
            # インデックス（指標名）でグループ化して平均をとる
            averaged_df = combined_df.groupby(combined_df.index).mean()
            # カラム順序を維持（groupbyでソートされる可能性があるため）
            if not folder_dfs[0].columns.empty:
                averaged_df = averaged_df.reindex(columns=folder_dfs[0].columns)
            
            folder_name = os.path.basename(folder)
            all_data[folder_name] = averaged_df

    if not all_data:
        print("有効なデータが見つかりませんでした。")
        return

    # 3. 可視化実行
    # 単一フォルダでも複数フォルダでも、指定された指標ごとにグラフを表示する
    visualize_comparison(all_data, TARGET_METRICS)

if __name__ == "__main__":
    main()
