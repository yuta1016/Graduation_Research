import os
import csv
from dataclasses import dataclass, field
from typing import List
import statistics
import pandas as pd

INPUT_DATA_FOLDER = "csv_with_mp3_path"
OUTPUT_FOLDER = "billboard_futures_info"
OUTPUT_FILENAME = "2008_2025_billboard_futures_info"

YEARS = ["2008", "2009", "2010", "2011", "2012", 
         "2013", "2014", "2015", "2016", "2017", 
         "2018", "2019", "2020", "2021", "2022", 
         "2023", "2024", "2025"]

# --- 1. データ構造の定義 (コンテナ) ---
@dataclass
class SongData:
    artist: str
    track: str
    url: str
    # スコアのリストを空リストで自動初期化
    scores: List[int] = field(default_factory=list)


class CalculateFeatures:
    def __init__(self):
        self.songs_db = {}
        self.features_list = []


    def get_csvfile_path(self):
        all_csvfile_path = []
        for dir_path, dirnames, filenames in os.walk(INPUT_DATA_FOLDER):
            #指定した年のフォルダーだけのcsvファイルを集める
            if any(year in dir_path for year in YEARS):
                for filename in filenames:
                    if filename.endswith(".csv"):
                        file_path = os.path.join(dir_path, filename)
                        all_csvfile_path.append(file_path)
        return all_csvfile_path
    

    #楽曲のもっているscoreを一旦集める
    def retrive_score(self, all_csv_path):
        for csv_path in all_csv_path:

            try:
                with open(csv_path, mode="r", encoding="utf-8") as file:
                    reader = csv.reader(file)
                    header = next(reader)
                    songs_info = list(reader)

                    for index, row in enumerate(songs_info):
                        #アーティスト名,曲名,順位,Spotify URL,アーティスト人気度,track人気度,MP3_Local_Path
                        artist = row[0].strip()
                        track = row[1].strip()
                        score = int(row[2].strip())
                        url = row[3].strip() #曲を識別するために使う

                        if url not in self.songs_db:
                            self.songs_db[url] = SongData(artist, track, url)
                        
                        # 既存でも新規でも、スコアを追加するだけ
                        self.songs_db[url].scores.append(score)
            except Exception as e:
                print(e)
        return self.songs_db
    


    def calculate_features(self, song: SongData):
        scores = song.scores
        n = len(scores)

        if n == 0: return

        # 1. Debut: 初登場時のスコア (リストの最初)
        debut = scores[0]

        # 2. Max: 最高スコア
        max_score = max(scores)

        # 3. Mean: 平均スコア
        mean_score = statistics.mean(scores)

        # 4. Length: チャートイン期間 (週数)
        length = n

        # 5. Sum: スコアの合計
        total_sum = sum(scores)

        # 6. Std: 標準偏差
        # データが1つしかない場合は偏差が計算できないため0とする
        if n > 1:
            std_dev = statistics.stdev(scores)
        else:
            std_dev = 0.0

        # 7. Skewness (歪度) & 8. Kurtosis (尖度)
        # 標準偏差が0（すべてのスコアが同じ）の場合は計算不能なので0とする
        skewness = 0.0
        kurtosis = 0.0

        if n > 2 and std_dev > 0:
            # 偏差の計算
            diffs = [x - mean_score for x in scores]
            
            # Skewness (歪度) の計算: Fisher-Pearson coefficient
            # m3 = sum((x-mean)^3) / n
            m3 = sum(d**3 for d in diffs) / n
            skewness = m3 / (std_dev**3)

            # Kurtosis (尖度) の計算: Excess Kurtosis (正規分布を0とするため -3 するのが一般的)
            # m4 = sum((x-mean)^4) / n
            m4 = sum(d**4 for d in diffs) / n
            kurtosis = (m4 / (std_dev**4)) - 3

        # 結果を辞書にまとめる
        feature_data = {
            "Artist": song.artist,
            "Track": song.track,
            "URL": song.url,
            "Debut": debut,
            "Max": max_score,
            "Mean": round(mean_score, 2),
            "Std": round(std_dev, 2),
            "Length": length,
            "Sum": total_sum,
            "Skewness": round(skewness, 2),
            "Kurtosis": round(kurtosis, 2)
        }

        # 保存
        self.features_list.append(feature_data)
        
        # 確認用出力
        # print(f"計算完了: {song.track} -> {feature_data}")


    def save_csv(self):
        os.makedirs(OUTPUT_FOLDER, exist_ok=True)
        save_path = os.path.join(OUTPUT_FOLDER, OUTPUT_FILENAME+".csv")

        try:
            df = pd.DataFrame(self.features_list)

            df.to_csv(save_path, index=False, encoding="utf-8-sig")

            print("●成功")
        except Exception as e:
            print(f"❌保存エラー : {e}")



def main():
    calculater = CalculateFeatures()
    print("ビルボードの特徴量を計算するよ")

    all_csv_path = calculater.get_csvfile_path()
    print(f"ファイル数: {len(all_csv_path)}")
    #print(all_csv_path)

    #以下のものが複数入った配列が帰ってくる
    #'https://': SongData(artist='幾田りら', track='Answer', url='https://', scores=[1, 1, 5]), 
    songs_info = calculater.retrive_score(all_csv_path)
    print(f"集計曲数: {len(songs_info)}")
    # print(songs_info)

    
    print("\n--- 特徴量計算開始 ---")
    #self.features_listに特徴量が以下の形式で保存されている
    #{'Artist': 'EXILE', 'Track': 'Ti Amo', 'URL': 'https://open.spotify.com/track/6YFewEWHZB04bJfGYgbYCC', 'Debut': 99, 'Max': 100, 'Mean': 76.75, 'Std': 26.99, 'Length': 8, 'Sum': 614, 'Skewness': -0.82, 'Kurtosis': -0.98}
    for song in songs_info.values():
        calculater.calculate_features(song)

    print(f"\n計算完了: {len(calculater.features_list)} 曲の特徴量を保存しました。")
    

    calculater.save_csv()


if __name__ == "__main__":
    main()