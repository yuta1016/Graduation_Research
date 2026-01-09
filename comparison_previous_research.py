import pandas as pd
import matplotlib.pyplot as plt
import os
import math

FOR_COMPARISON_CSV = "./result_SVM/comparison_previous_research/results_2009_2014.csv"

FOR_COMPARISON_FOLDER = "./result_SVM/traing_70_per/"
INCLUDE_SPOTIFY_POPULARITY = False

METRICS = ["Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]


def process_csv(file_path, ax):
    df = pd.read_csv(file_path, index_col=0)

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
    sliced_df.plot(kind='bar', ax=ax, color=colors, width=0.8, rot=0)
    
    ax.grid(axis='y', linestyle='--', alpha=0.7)
    ax.set_axisbelow(True)
    ax.set_ylabel("Balanced accuracy")
    ax.set_ylim(0, 0.70)
    ax.tick_params(axis='x', labelsize=8)
    
    # タイトル設定: ファイル名の_の後ろの方を取得 (例: results_2009_2014.csv -> 2014)
    filename = os.path.basename(file_path)
    title_text = os.path.splitext(filename)[0].split('_')[-1]
    ax.set_title(title_text)

    ax.legend(loc='upper right', fontsize='x-small')

def main():
    files_to_process = []
    for root, _, files in os.walk(FOR_COMPARISON_FOLDER):
        for file in files:
            if file.endswith(".csv"):
                files_to_process.append(os.path.join(root, file))
    
    # 年代順にソート (ファイル名の末尾の年を利用)
    files_to_process.sort(key=lambda f: int(os.path.splitext(os.path.basename(f))[0].split('_')[-1]))

    if not files_to_process:
        return

    num_files = len(files_to_process)
    cols = 2
    rows = math.ceil(num_files / cols)

    fig, axes = plt.subplots(rows, cols, figsize=(15, 5 * rows), constrained_layout=True)
    axes_flat = axes.flatten()

    for i, file_path in enumerate(files_to_process):
        process_csv(file_path, axes_flat[i])

    # 余ったサブプロットを非表示
    for j in range(i + 1, len(axes_flat)):
        axes_flat[j].axis('off')

    plt.show()
    


if __name__ == "__main__":
    main()