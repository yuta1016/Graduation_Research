import os
import csv
from dataclasses import dataclass, field
from typing import List

INPUT_DATA_FOLDER = "exact_match_csv_of_spotify_info"
OUTPUT_FOLDER = "billboard_futures_info"

# --- 1. データ構造の定義 (コンテナ) ---
@dataclass
class SongData:
    artist: str
    track: str
    url: str
    # スコアのリストを空リストで自動初期化
    scores: List[int] = field(default_factory=list)


class CalculateFutures:
    def __init__(self):
        self.songs_db = {}        

    def get_csvfile_path(self):
        all_csvfile_path = []
        for dir_path, _, filenames in os.walk(INPUT_DATA_FOLDER):
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
                        artist = row[0].strip()
                        track = row[1].strip()
                        score_str = row[2].strip()
                        url = row[3].strip() #曲を識別するために使う

                        try:
                            score = int(score_str)
                        except ValueError:
                            return

                        if url not in self.songs_db:
                            self.songs_db[url] = SongData(artist, track, url)
                        
                        # 既存でも新規でも、スコアを追加するだけ
                        self.songs_db[url].scores.append(score)
            except Exception as e:
                print(e)
        return self.songs_db
    
    def _save_song_info():
        print("Aaa")

    def average(song):
        if len(song.scores) > 0:
            average_score = sum(song.scores) / len(song.scores)
            _save_song_info
            print(f"artist名: {song.artist} - 曲名: {song.track} / 平均スコア: {average_score:.2f}")



def main():
    calculater = CalculateFutures()

    print("ビルボードの特徴量を計算するよ")

    all_csv_path = calculater.get_csvfile_path()
    print(len(all_csv_path))

    songs_info = calculater.retrive_score(all_csv_path)
    print(len(songs_info))
    print(songs_info)

    for song in songs_info.values():
        # 計算例: 平均スコアを出す
        calculater.average(song)


if __name__ == "__main__":
    main()