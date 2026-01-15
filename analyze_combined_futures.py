#フォルダ内のcsvを読み込み,complexity+mfccのみを抽出して指標ごとに年代を決めて棒グラフで比較する

import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
import analyze_results as ar

TARGET_METRICS = ["Average", "Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]
INPUT_FOLUDER = "./result_SVM/grid_search/grid_search_rbf"



def make_dataframe(df):
    #指標ごとにデータフレームを作成する
    final_df = pd.DataFrame()

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
    
    return final_df

def main():
    folders = ar.select_folders()
    if not folders:
        print("フォルダが選択されませんでした。")
        return
    
    csv_files = glob.glob(os.path.join(INPUT_FOLUDER, "*.csv"))

    for csv_file in csv_files:
        df, metadata = ar.load_csv_data(csv_file)
        final_df = ar.make_dataframe(df)


if __name__ == "__main__":
    main()