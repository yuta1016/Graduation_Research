import os
import csv

class FeaturesExcelSaver:
    def find_input_csv_files(self, input_root):
        """指定されたルートディレクトリ以下のすべてのCSVファイルを見つけてパスのリストを返す"""
        csv_files = []
        print(f"ディレクトリ '{input_root}' 以下からCSVファイルを検索中...")
        
        for dirpath, _, filenames in os.walk(input_root):
            for filename in filenames:
                if filename.endswith('.csv'):
                    full_path = os.path.join(dirpath, filename)
                    csv_files.append(full_path)
        return csv_files
    

    def retrive_mp3_path(self, all_csv_files, unmber_of_mp3_path, number_of_url):
        mp3_paths = {}
        for_excel_column_names = {"Artist", "Track", "URL", "MP3_Path"}
        csv_data = []

        for csv_file in all_csv_files:
            with open(csv_file, mode="r", encoding="utf-8") as file:
                reader = csv.reader(file)
                header = next(reader)
                for row in reader:
                    artist = row[0].strip()
                    track = row[1].strip()
                    url = row[number_of_url].strip()
                    mp3_path = row[unmber_of_mp3_path].strip()
                    url = row[number_of_url].strip()
                    mp3_paths[url] = mp3_path

                    csv_data.append({
                        "Artist": artist,
                        "Track": track,
                        "URL": url,
                        "MP3_Path": mp3_path
                        })
        return mp3_paths, csv_data
    


    def save_features_to_excel(self, features_list, csv_data, output_excel_path, file_name):
        import pandas as pd
        import os

        # 1. 保存先ディレクトリがなければ作成
        if not os.path.exists(output_excel_path):
            os.makedirs(output_excel_path)
            print(f"ディレクトリ作成: {output_excel_path}")

        # 2. 特徴量の辞書からDataFrameを作成
        # {'URL': {'Feat1': 0.1, ...}} の形を想定
        df_features = pd.DataFrame.from_dict(features_list, orient='index')
        
        # インデックス（URL）を通常の列に戻す
        df_features.index.name = 'URL'
        df_features.reset_index(inplace=True)

        # 3. 元のCSVデータ(csv_data)をDataFrameとして準備
        # csv_dataが既にDataFrameならコピー、リストなら変換
        if isinstance(csv_data, pd.DataFrame):
            df_meta = csv_data.copy()
        else:
            df_meta = pd.DataFrame(csv_data)

        # 4. 「URL」をキーにして結合 (Merge)
        # on='URL': URL列が一致する行同士をくっつけます
        # how='inner': 両方に存在するデータ（計算に成功した曲）のみ残します
        try:
            merged_df = pd.merge(df_meta, df_features, on='URL', how='inner')
        except KeyError as e:
            print(f"❌ 結合エラー: {e}")
            print("csv_dataと特徴量データの両方に 'URL' という名前の列(またはキー)が必要です。")
            return

        # 5. パスの結合
        full_path = os.path.join(output_excel_path, file_name)

        # 6. Excelファイルに保存
        try:
            merged_df.to_excel(full_path, index=False)
            print(f"✅ 特徴量とメタデータを結合して保存しました: '{full_path}'")
            print(f"   データサイズ: {merged_df.shape[0]}曲, 列数: {merged_df.shape[1]}")
        except Exception as e:
            print(f"❌ 保存中にエラーが発生しました: {e}")

    if __name__ == "__main__":
        print("このスクリプトはモジュールとして使用されることを想定しています。")