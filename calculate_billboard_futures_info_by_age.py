import os
import csv
from dataclasses import dataclass, field
from typing import List
import statistics
import pandas as pd
import re

INPUT_DATA_FOLDER = "csv_with_mp3_path"
OUTPUT_FOLDER = "billboard_futures_info_by_age"
OUTPUT_FILENAME = "billboard_features_by_age"

# TRAIN_CSV = "train_2008_2017.csv"
# VAL_CSV = "val_2018_2021.csv"
# TEST_CSV = "test_2022_2025.csv"

# 年代設定
# YEARS_TRAIN = ["2008", "2009", "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017"]
# YEARS_VAL   = ["2018", "2019", "2020", "2021"]
# YEARS_TEST  = ["2022", "2023", "2024", "2025"]

YEARS_TRAIN = ["2020", "2021", "2022"]
YEARS_VAL   = ["2023"]
YEARS_TEST  = ["2024", "2025"]

TRAIN_CSV = "train_2020_2022.csv"
VAL_CSV = "val_2023.csv"
TEST_CSV = "test_2024_2025.csv"

THRESHOLD_YEAR_TRAIN_END = 2022
THRESHOLD_YEAR_VAL_END   = 2023

# 全ての対象年
ALL_YEARS = YEARS_TRAIN + YEARS_VAL + YEARS_TEST

# --- 1. データ構造の定義 (コンテナ) ---
@dataclass
class SongData:
    artist: str
    track: str
    url: str
    mp3_path: str
    artist_popularity: int
    track_popularity: int
    # ★追加: 初登場年を記録（最初はありえない大きな数字にしておく）
    debut_year: int = 9999
    # スコアのリストを空リストで自動初期化
    scores: List[int] = field(default_factory=list)


class CalculateFeatures:
    def __init__(self):
        self.songs_db = {}
        self.features_list = []

    def get_csvfile_path(self):
        all_csvfile_path = []
        for dir_path, dirnames, filenames in os.walk(INPUT_DATA_FOLDER):
            # 指定した年のフォルダーだけのcsvファイルを集める
            if any(year in dir_path for year in ALL_YEARS):
                for filename in filenames:
                    if filename.endswith(".csv"):
                        file_path = os.path.join(dir_path, filename)
                        all_csvfile_path.append(file_path)
        return all_csvfile_path
    
    # パスから年を抽出するヘルパー関数
    def _extract_year_from_path(self, path):
        # パスの中に '2008' ～ '2025' の文字列が含まれていればそれを返す
        for year in ALL_YEARS:
            if year in path:
                #print(f"Extracted year {year} from path {path}")
                return int(year)
        return 9999

    # 楽曲のもっているscoreを一旦集める
    def retrive_score(self, all_csv_path):
        for csv_path in all_csv_path:
            # ★現在のCSVが何年のものか取得
            current_year = self._extract_year_from_path(csv_path)

            try:
                with open(csv_path, mode="r", encoding="utf-8") as file:
                    reader = csv.reader(file)
                    header = next(reader)
                    songs_info = list(reader)

                    for index, row in enumerate(songs_info):
                        artist = row[0].strip()
                        track = row[1].strip()
                        score = int(row[2].strip())
                        url = row[3].strip() 
                        artist_popularity = int(row[4].strip())
                        track_popularity = int(row[5].strip())
                        mp3_path = row[6].strip()

                        if url not in self.songs_db:
                            self.songs_db[url] = SongData(artist, track, url, mp3_path, artist_popularity, track_popularity)
                        # ★ デビュー年の更新: 
                        # 登録されている年より、今のファイルの年が古ければ更新（初登場年を特定するため）
                        if current_year < self.songs_db[url].debut_year:
                            self.songs_db[url].debut_year = current_year

                        # スコアを追加
                        self.songs_db[url].scores.append(score)

            except Exception as e:
                print(f"Error reading {csv_path}: {e}")
        return self.songs_db
    

    def calculate_features(self, song: SongData):
        scores = song.scores
        n = len(scores)
        if n == 0: return

        # 特徴量計算（変更なし）
        debut = scores[0]
        max_score = max(scores)
        mean_score = statistics.mean(scores)
        length = n
        total_sum = sum(scores)
        
        if n > 1:
            std_dev = statistics.stdev(scores)
        else:
            std_dev = 0.0

        skewness = 0.0
        kurtosis = 0.0

        if n > 2 and std_dev > 0:
            diffs = [x - mean_score for x in scores]
            m3 = sum(d**3 for d in diffs) / n
            skewness = m3 / (std_dev**3)
            m4 = sum(d**4 for d in diffs) / n
            kurtosis = (m4 / (std_dev**4)) - 3

        feature_data = {
            "Artist": song.artist,
            "Track": song.track,
            "URL": song.url,
            "MP3_Path": song.mp3_path,
            "Debut_Year": song.debut_year, # ★保存用に年も追加しておくと便利
            "Debut_Score": debut,
            "Max": max_score,
            "Mean": round(mean_score, 2),
            "Std": round(std_dev, 2),
            "Length": length,
            "Sum": total_sum,
            "Skewness": round(skewness, 2),
            "Kurtosis": round(kurtosis, 2),
            "Artist_Popularity": song.artist_popularity,
            "Track_Popularity": song.track_popularity
        }

        self.features_list.append(feature_data)


    # ★ここを大幅変更：データを分割して保存
    def save_split_csv(self):
        os.makedirs(OUTPUT_FOLDER, exist_ok=True)

        # 全データをDataFrame化
        df = pd.DataFrame(self.features_list)
        
        # カラムの並び順定義
        cols = ["Artist", "Track", "URL", "MP3_Path", "Debut_Year", "Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis", "Artist_Popularity", "Track_Popularity"]
        df = df[cols] # 並び替え

        # --- 分割ロジック ---
        # 1. Train (2008-2019)
        # Debut_Yearが2019以下のもの
        df_train = df[df["Debut_Year"] <= THRESHOLD_YEAR_TRAIN_END]
        
        # 2. Validation (2020-2022)
        # Debut_Yearが2020以上、かつ2022以下のもの
        df_val = df[(df["Debut_Year"] > THRESHOLD_YEAR_TRAIN_END) & (df["Debut_Year"] <= THRESHOLD_YEAR_VAL_END)]
        # 3. Test (2023-2025)
        # Debut_Yearが2023以上のもの
        df_test = df[df["Debut_Year"] > THRESHOLD_YEAR_VAL_END]

        # --- 保存 ---
        try:
            # Train保存
            train_path = os.path.join(OUTPUT_FOLDER, TRAIN_CSV)
            df_train.to_csv(train_path, index=False, encoding="utf-8-sig")
            print(f"✅ 学習データ保存完了: {train_path} ({len(df_train)}曲)")

            # Val保存
            val_path = os.path.join(OUTPUT_FOLDER, VAL_CSV)
            df_val.to_csv(val_path, index=False, encoding="utf-8-sig")
            print(f"✅ 検証データ保存完了: {val_path} ({len(df_val)}曲)")

            # Test保存
            test_path = os.path.join(OUTPUT_FOLDER, TEST_CSV)
            df_test.to_csv(test_path, index=False, encoding="utf-8-sig")
            print(f"✅ テストデータ保存完了: {test_path} ({len(df_test)}曲)")

        except Exception as e:
            print(f"❌保存エラー : {e}")


def main():
    calculater = CalculateFeatures()
    print("ビルボードの特徴量を計算し、時系列で分割保存します")

    all_csv_path = calculater.get_csvfile_path()
    print(f"読込対象ファイル数: {len(all_csv_path)}")

    songs_info = calculater.retrive_score(all_csv_path)
    print(f"集計ユニーク曲数: {len(songs_info)}")
    
    print("\n--- 特徴量計算開始 ---")
    for song in songs_info.values():
        calculater.calculate_features(song)

    print(f"計算完了。保存処理に移ります...")
    
    # 分割保存を実行
    calculater.save_split_csv()


if __name__ == "__main__":
    main()