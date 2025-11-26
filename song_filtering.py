"""
1,2週しかランクインしなかった曲を除外するフィルタリング処理
"""
import os
import csv
import pandas as pd
from collections import Counter
import shutil # フォルダ操作のために追加
from typing import List, Tuple, Set

# --- 設定 ---
INPUT_ROOT = './test_output/'
OUTPUT_ROOT = './filtered_billboard_charts/'
MIN_APPEARANCE_COUNT = 2
# -----------

class SongFilter:
    """
    複数のCSVファイルから楽曲データを集計し、
    指定された基準 (MIN_APPEARANCE_COUNT) を満たす楽曲のみをフィルタリングするクラス。
    """
    def __init__(self, input_root, min_count):
        self.input_root = input_root
        self.min_count = min_count
        self.song_counts = Counter()
        # { (アーティスト名, 曲名): [全データ行] }
        self.all_song_data = {}
        self.output_header = None


    def find_input_csv_files(self):
        """指定されたルートディレクトリ以下のすべてのCSVファイルを見つけてパスのリストを返す"""
        csv_files = []
        print(f"ディレクトリ '{self.input_root}' 以下からCSVファイルを検索中...")
        
        for dirpath, _, filenames in os.walk(self.input_root):
            for filename in filenames:
                if filename.endswith('.csv'):
                    full_path = os.path.join(dirpath, filename)
                    csv_files.append(full_path)
        return csv_files


    def _process_single_file(self, file_path):
        """単一のCSVファイルを処理し、カウントとデータ格納を行う (内部メソッド)"""
        try:
            with open(file_path, mode='r', encoding='utf-8') as f:
                reader = csv.reader(f)
                
                # 初回のみヘッダーを保持
                if self.output_header is None:
                    self.output_header = next(reader)
                else:
                    next(reader) # それ以外はヘッダーをスキップ
                

                # CSVの構造: [アーティスト名, 曲名, 順位, SpotifyURL, アーティスト人気度, track人気度]
                for row in reader:
                    #アーティスト名はrow[0]、曲名はrow[1]、スコアはrow[2],,,
                    artist = row[0].strip()
                    track = row[1].strip()
                    
                    song_key = (artist, track)
                    
                    self.song_counts[song_key] += 1
                    
                    if song_key not in self.all_song_data:
                        self.all_song_data[song_key] = []
                    
                    self.all_song_data[song_key].append(row)
                        
        except Exception as e:
            print(f"⚠️ 警告: ファイル {file_path} の読み込み中にエラーが発生しました: {e}")


    def aggregate_and_filter(self, all_csv_files: List[str]) -> Tuple[List[str], Set[Tuple[str, str]]]:
        """全ファイルを処理し、基準を満たした楽曲のキーのセットを返す"""

        print("--- 1. 全ファイルの楽曲出現回数を集計中 ---")
        for file_path in all_csv_files:
            self._process_single_file(file_path)

        print(f"総ユニーク楽曲数: {len(self.song_counts)}件")
        
        # 2. フィルタリングキーの特定
        retained_song_keys = set()
        
        for song_key, count in self.song_counts.items():
            if count >= self.min_count:
                retained_song_keys.add(song_key)

        print(f"✅ フィルタリング完了: 基準 ({self.min_count}回以上) を満たしたユニーク楽曲数: {len(retained_song_keys)}件")
        
        return self.output_header, retained_song_keys



def replicate_and_save(input_root: str, output_root: str, all_csv_files: List[str], 
                       header: List[str], retained_keys: Set[Tuple[str, str]]):
    
    print("\n--- 2. フィルタリング結果のファイル構造再現と保存 ---")
    
    for input_file_path in all_csv_files:
        current_file_data = []
        
        # 1. ファイルを読み込み、フィルタリング
        try:
            with open(input_file_path, mode='r', encoding='utf-8') as infile:
                reader = csv.reader(infile)
                next(reader) # ヘッダーをスキップ
                
                for row in reader:
                    if len(row) < 2: continue

                    artist = row[0].strip()
                    track = row[1].strip()
                    song_key = (artist, track)
                    
                    # 基準を満たした楽曲のみを追加
                    if song_key in retained_keys:
                        current_file_data.append(row)
                        
        except Exception as e:
            print(f"⚠️ 警告: ファイル {input_file_path} の再読み込み中にエラーが発生しました: {e}")
            continue

        if not current_file_data:
            # フィルタリングの結果、データが残らなかった場合はファイルを作成しない
            continue

        # 2. フォルダ構造の再現とパスの構築
        relative_path = os.path.relpath(os.path.dirname(input_file_path), input_root)
        output_dir = os.path.join(output_root, relative_path)
        os.makedirs(output_dir, exist_ok=True)
        
        # 出力ファイル名を構築
        base_filename = os.path.basename(input_file_path).replace('.csv', '_filtered.csv')
        output_file_path = os.path.join(output_dir, base_filename)
        
        # 3. 書き出し
        try:
            with open(output_file_path, mode='w', newline='', encoding='utf-8') as outfile:
                writer = csv.writer(outfile)
                writer.writerow(header)
                writer.writerows(current_file_data)
            
            print(f"✅ 保存成功: {output_file_path}")
        except Exception as e:
            print(f"エラー: ファイル '{output_file_path}' への書き出し中に問題が発生しました。{e}")



def main():
    if not os.path.exists(INPUT_ROOT):
        print(f"エラー: 入力ディレクトリ '{INPUT_ROOT}' が見つかりません。プログラムを終了します。")
        return
        
    song_processor = SongFilter(INPUT_ROOT, MIN_APPEARANCE_COUNT)

    # ROOT_INPUTから全てのcsvファイルの名前を取得
    all_csv_files = song_processor.find_input_csv_files()

    if not all_csv_files:
        print("エラー: 処理対象のCSVファイルが見つかりませんでした。")
        return
    print(f"処理対象ファイル総数: {len(all_csv_files)}件")
    
    # header: [アーティスト名, 曲名, 順位, SpotifyURL, アーティスト人気度, track人気度
    # retained_keys: artist, trackのタプルセット。週に3回以上出現した曲
    header, retained_keys = song_processor.aggregate_and_filter(all_csv_files)
    
    # フィルタリングされたキーを使って、ファイル構造を再現しながらデータを出力
    replicate_and_save(INPUT_ROOT, OUTPUT_ROOT, all_csv_files, header, retained_keys)

    print("\n=======================================")
    print("🎉 すべてのファイルの処理が完了しました。")
    print("=======================================")

if __name__ == "__main__":
    main()