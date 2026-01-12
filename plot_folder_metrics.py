import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import glob
import os
import tkinter as tk
from tkinter import filedialog

def main():
    # tkinterのルートウィンドウを作成し、非表示にする（ダイアログのみ表示するため）
    root = tk.Tk()
    root.withdraw()

    print("フォルダを選択してください...")
    # フォルダ選択ダイアログを表示
    folder_path = filedialog.askdirectory(title="CSVファイルが含まれるフォルダを選択")

    if not folder_path:
        print("フォルダが選択されませんでした。")
        return

    # フォルダ内のCSVファイルを検索
    csv_files = glob.glob(os.path.join(folder_path, "*.csv"))

    if not csv_files:
        print("選択されたフォルダにCSVファイルが見つかりませんでした。")
        return

    print(f"{len(csv_files)} 個のCSVファイルが見つかりました。集計を開始します...")

    data_list = []
    
    # 除外するメタデータのキーワード（CSVの下部に含まれる行など）
    exclude_keywords = [
        'training', 'val', 'test', 'Data Shapes', 
        'traing_data(%)', 'Train', 'Val', 'Test'
    ]

    for file in csv_files:
        try:
            # CSVを読み込む
            df = pd.read_csv(file)
            
            # 1列目のカラム名を統一（Target_Metricと仮定）
            if df.columns[0] != 'Target_Metric':
                df.rename(columns={df.columns[0]: 'Target_Metric'}, inplace=True)

            # 数値データのみを抽出するための処理
            # 実験データのカラム（2列目以降）を取得
            exp_cols = df.columns[1:]
            
            # 数値に変換（変換できない文字はNaNにする）
            for col in exp_cols:
                df[col] = pd.to_numeric(df[col], errors='coerce')
            
            # NaNを含む行（メタデータや空行）を削除
            df_clean = df.dropna(subset=exp_cols)
            
            # 特定のキーワードを含む行をさらに除外
            df_clean = df_clean[~df_clean['Target_Metric'].isin(exclude_keywords)]
            
            data_list.append(df_clean)
            
        except Exception as e:
            print(f"エラーが発生しました ({file}): {e}")

    if not data_list:
        print("有効なデータが見つかりませんでした。")
        return

    # 全データを結合
    combined_df = pd.concat(data_list)

    # 指標（Target_Metric）ごとに平均を計算
    averaged_df = combined_df.groupby('Target_Metric', as_index=False).mean(numeric_only=True)

    # 元の順序（Debut_Score, Kurtosis... Averageの順）を保持したい場合、
    # 最初のファイルの順序を使ってソートを行う
    if data_list:
        original_order = data_list[0]['Target_Metric'].unique()
        # 存在するカテゴリのみを使用
        valid_order = [x for x in original_order if x in averaged_df['Target_Metric'].values]
        
        averaged_df['Target_Metric'] = pd.Categorical(
            averaged_df['Target_Metric'], 
            categories=valid_order, 
            ordered=True
        )
        averaged_df = averaged_df.sort_values('Target_Metric')

    print("集計結果:")
    print(averaged_df)

    # プロット用にデータを整形（Long format）
    plot_data = averaged_df.melt(id_vars='Target_Metric', var_name='Experiment', value_name='Score')

    # グラフの作成
    plt.figure(figsize=(14, 8))
    sns.set_style("whitegrid")
    
    # 棒グラフを描画
    ax = sns.barplot(data=plot_data, x='Target_Metric', y='Score', hue='Experiment', palette='Set1')
    
    # 各棒グラフの上に値を表示
    for container in ax.containers:
        ax.bar_label(container, fmt='%.3f', padding=3, fontsize=9)
    
    plt.title('Average Metrics by Experiment (Folder Aggregation)', fontsize=16)
    plt.xlabel('Metric', fontsize=12)
    plt.ylabel('Score', fontsize=12)
    plt.xticks(rotation=45)
    plt.legend(title='Experiment', bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    
    # グラフを表示
    plt.show()

if __name__ == "__main__":
    main()