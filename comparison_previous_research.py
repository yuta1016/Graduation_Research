#一つのcsvを読み込み、実験ごとに指標を棒グラフで比較する

import pandas as pd
import matplotlib.pyplot as plt
import os

FOR_COMPARISON_CSV = "./result_SVM/grid_search/grid_search_rbf/results_2009_2014.csv"

#FOR_COMPARISON_FOLDER = "./result_SVM/traing_70_per/"
INCLUDE_SPOTIFY_POPULARITY = False

METRICS = ["Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]


def calculate_average(file):
    print("Calculating average for each metric...")
    # CSVを読み込む
    df = pd.read_csv(file)
    
    # 1列目のカラム名を統一（Target_Metricと仮定）
    if df.columns[0] != 'Target_Metric':
        df.rename(columns={df.columns[0]: 'Target_Metric'}, inplace=True)

    # METRICSに含まれる行のみを抽出
    df_clean = df[df['Target_Metric'].isin(METRICS)].copy()

    # 実験データのカラム（2列目以降）を取得
    exp_cols = df_clean.columns[1:]
    
    # 数値に変換（変換できない文字はNaNにする）
    for col in exp_cols:
        df_clean[col] = pd.to_numeric(df_clean[col], errors='coerce')
    
    # 各実験（カラム）ごとの平均を計算
    averages = df_clean[exp_cols].mean()
    print("--- Experiment Averages ---")
    print(averages)
    return averages


def process_csv(file_path):
    df = pd.read_csv(FOR_COMPARISON_CSV, index_col=0)

    #指標以外を削除 (Average行より前を取得して8つの指標にする)
    if 'Average' in df.index:
        end_index = df.index.get_loc('Average')
        sliced_df = df.iloc[0:end_index]
    else:
        sliced_df = df

    sliced_df.index.name = "Metric"
    
    # カラム名の変更
    sliced_df = sliced_df.rename(columns={"Exp3: Combined_All": "Complexity+MFCC"})
    sliced_df = sliced_df.rename(columns={"Exp2: Group_MFCC": "MFCC"})
    sliced_df = sliced_df.rename(columns={"Exp1: Group_Complexity": "Complexity"})

    #8個の指標の順番を変える
    sliced_df = sliced_df.reindex(METRICS)

    if not INCLUDE_SPOTIFY_POPULARITY:
        if "Exp4: Spotify_Popularity" in sliced_df.columns:
            sliced_df = sliced_df.drop(columns=["Exp4: Spotify_Popularity"])

    # 色の設定
    color_dict = {
        "Complexity": "blue",
        "MFCC": "grey",
        "Complexity+MFCC": "red",
        "Exp4: Spotify_Popularity": "yellow"
    }
    colors = [color_dict.get(col, 'gray') for col in sliced_df.columns]

    # 棒グラフの描画
    ax = sliced_df.plot(kind='bar', figsize=(12, 6), color=colors, width=0.8)
    
    ax.grid(axis='y', linestyle='--', alpha=0.7)
    ax.set_axisbelow(True)
    plt.ylabel("Balanced accuracy")
    plt.ylim(0, 0.70)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0)
    plt.tight_layout()
    plt.show()

def main():
    #指定したcsvファイルの各指標ごとの平均を計算
    calculate_average(FOR_COMPARISON_CSV)

    #指定したcsvファイルを読み込み、各指標ごとの棒グラフを作成
    #process_csv(FOR_COMPARISON_CSV)
    


if __name__ == "__main__":
    main()