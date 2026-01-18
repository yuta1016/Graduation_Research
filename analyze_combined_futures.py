#フォルダ内のcsvを読み込み,complexity+mfccのみを抽出して指標ごとに年代を決めて棒グラフで比較する

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import glob
import analyze_results as ar
from matplotlib.patches import Patch

TARGET_METRICS = ["Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]
INPUT_FOLDER = "./result_SVM/grid_search/grid_search_rbf"

# ソート設定: True=スコア順(大きさ順), False=年代順
SORT_BY_SCORE = True

#TARGET_EXP = "Exp3: Combined_All"
#TARGET_EXP = "Exp1: Group_Complexity"
TARGET_EXP = "Exp2: Group_MFCC"

def main():
    # フォルダ内のCSVファイルを取得
    csv_files = glob.glob(os.path.join(INPUT_FOLDER, "*.csv"))
    csv_files = [f for f in csv_files if "results_2020_2025.csv" not in f]

    if not csv_files:
        print(f"CSV files not found in {INPUT_FOLDER}")
        return

    all_data = []

    for csv_file in csv_files:
        # analyze_resultsの関数を使ってロード
        df, metadata = ar.load_csv_data(csv_file)
        
        if df is None or df.empty:
            continue

        # Complexity+MFCCの列 ("Exp3: Combined_All") を抽出
        if TARGET_EXP not in df.columns:
            continue

        # 必要な指標のみフィルタリング
        df_filtered = df.loc[df.index.isin(TARGET_METRICS), [TARGET_EXP]].copy()
        
        if df_filtered.empty:
            continue

        # Period (年代) の特定
        period = metadata.get("test_period", "Unknown")
        if period == "Unknown":
            try:
                basename = os.path.basename(csv_file)
                # ファイル名規則: results_YYYY_YYYY.csv -> 後ろの年を取得
                parts = basename.replace("results_", "").replace(".csv", "").split("_")
                if len(parts) >= 2:
                    period = parts[-1]
            except:
                pass
        
        # データ整形 (Metric, Score, Period)
        df_filtered.index.name = 'Metric'
        df_reset = df_filtered.reset_index()
        df_reset.columns = ['Metric', 'Score']
        df_reset['Period'] = period
        
        all_data.append(df_reset)

    if not all_data:
        print("有効なデータが見つかりませんでした。")
        return

    final_df = pd.concat(all_data, ignore_index=True)

    # 年代順のリスト作成（色固定用）
    unique_periods = final_df['Period'].unique()
    def get_year(p):
        try:
            return int(p)
        except:
            return 9999
    sorted_periods = sorted(unique_periods, key=get_year)

    # カラーパレットの固定
    palette = sns.color_palette("viridis", n_colors=len(sorted_periods))
    period_to_color = dict(zip(sorted_periods, palette))

    # グラフ描画設定 (1行9列)
    fig, axes = plt.subplots(1, len(TARGET_METRICS), figsize=(20, 6), sharey=True)
    
    if len(TARGET_METRICS) == 1:
        axes = [axes]

    for i, metric in enumerate(TARGET_METRICS):
        ax = axes[i]
        metric_df = final_df[final_df['Metric'] == metric]
        
        if metric_df.empty:
            continue

        # ソート順決定
        if SORT_BY_SCORE:
            # スコア順 (降順)
            period_scores = metric_df.groupby('Period')['Score'].mean()
            hue_order = period_scores.sort_values(ascending=True).index.tolist()
        else:
            # 年代順
            hue_order = sorted(metric_df['Period'].unique(), key=get_year)

        #debut_scoreをdebutに変更
        metric_df['Metric'] = metric_df['Metric'].replace({'Debut_Score': 'Debut'})
            
        sns.barplot(
            data=metric_df,
            x='Metric',
            y='Score',
            hue='Period',
            hue_order=hue_order,
            palette=period_to_color,
            ax=ax,
        )
        
        # 装飾
        ax.set_xlabel("") #デフォルトである小さいxラベルを消す
        ax.tick_params(axis='x', labelsize=25)
        ax.tick_params(axis='y', labelsize=25)
        if ax.get_legend():
            ax.get_legend().remove() # 個別の凡例は削除
        
        if i == 0:
            ax.set_ylabel("Balanced Accuracy", fontsize=30)
        else:
            ax.set_ylabel("")


    # 右の年代の表
    legend_elements = [Patch(facecolor=period_to_color[p], label=p) for p in sorted_periods]
    fig.legend(handles=legend_elements, title="Period", loc='center right', bbox_to_anchor=(1.0, 0.5), fontsize=20)#0.98, 0.5

    # タイトル
    # plt.suptitle(f"{TARGET_EXP}", fontsize=30)
    # plt.suptitle(f"Exp3: Complexity+MFCC", fontsize=30)
    plt.suptitle(f"{TARGET_EXP}", fontsize=30)

    plt.ylim(0.3, 0.70) # 正答率なので0~1
    
    # Y軸の目盛りに0.58を追加
    axes[0].set_yticks([0.3, 0.4, 0.5, 0.55, 0.58, 0.6, 0.7])

    for ax in axes:#0.5に線を引く
        ax.axhline(0.58, color='red', linestyle='--', linewidth=3)
        ax.axhline(0.55, color='black', linestyle='--', linewidth=3)
        ax.axhline(0.50, color='blue', linestyle='--', linewidth=3)
    plt.tight_layout(rect=[0, 0, 0.90, 1]) # 右側に凡例用のスペースを空ける
    plt.show()

if __name__ == "__main__":
    main()