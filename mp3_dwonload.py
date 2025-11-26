"""
https://global.php.cn/ja/faq/1796615471.html

https://qiita.com/daisei0311/items/3b32a16faf3c2de826b9

stdlを使うときにmp3で落とす方法
"""
import subprocess
import os
import time
import csv
from typing import List, Set, Tuple, Dict

#INPUT_ROOT = "./csv_of_spotify_info/"
INPUT_ROOT = "./filtered_billboard_charts/"
OUTPUT_ROOT = "./csv_with_mp3_path/"
DOWNLOAD_DIR = "./downloaded_mp3/"


def download_spotify_mp3():

    print("Spotify to MP3 Downloader")

 

    content_type = input("What do you want to download? (Enter 'track', 'playlist', or 'album'): ").strip().lower()

 

    # if content_type not in ['track', 'playlist', 'album']:

    #     print("Invalid choice. Please enter 'track', 'playlist', or 'album'.")

    #     return

 

    spotify_url = "https://open.spotify.com/track/364JzOajs76bJymjHm3sVY"

 

    try:

        print(f"\nDownloading {content_type} as MP3...")

        subprocess.run(["spotdl", spotify_url])

        print(f"\nDownload of {content_type} completed in MP3 format!\n")

 

    except Exception as e:

        print(f"An error occurred: {e}")


class Mp3Downloader:
    def __init__(self):
        self.input_root = INPUT_ROOT
        self.download_dir = DOWNLOAD_DIR
        self.output_root = OUTPUT_ROOT

        self.song_info = {}
        self.output_header = None
        self.track_identifiers = set() # artist - song titel を格納

        self.downloaded_files = dict()# { URL: MP3の最終パス }


    def find_input_csvs(self) -> Dict[str, Tuple[str, str]]:
        csv_files = []
        for dir_path, _, filenames in os.walk(self.input_root):
            for filename in filenames:
                if filename.endswith(".csv"):
                    full_path = os.path.join(dir_path, filename)
                    csv_files.append(full_path)

        return csv_files


    # mp3をダウンロードするとき、重複するのを防ぐために、フィルタリングする
    # レスポンス: {artist, track : (spotify_url)}
    def collect_unique_urls(self, all_csv_files: List[str]) -> Set[str]:
        artist_track_urls = dict()

        for csv_file in all_csv_files:
            try:
                with open(csv_file, mode="r", encoding="utf-8") as f:
                    reader = csv.reader(f)

                    if self.output_header is None: self.output_header = next(reader)
                    else: next(reader)

                    for row in reader:
                        artist = row[0].strip()
                        track = row[1].strip
                        unique_url = row[2].strip()

                        artist_track_urls[unique_url] = artist, track

            except Exception as e:
                print(f"⚠️ 警告: ファイル {csv_file} の読み込み中にエラーが発生しました: {e}")
        
        print(f"✅ 合計 {len(artist_track_urls)} 個の一意のSpotifyトラックURLが収集されました。")
        return artist_track_urls


    def execute_download(self, url_with_info: Dict[str, Tuple[str, str]]):
        print("⬇️ MP3のダウンロードを開始しています...")

        for url, (artist, track) in url_with_info.items():
            filename = f"{artist}_{track}.mp3"
            mp3_path = os.path.join(self.download_dir, filename)

            try:
                command = [
                    "spotdl",
                    url,
                    "--output",
                    mp3_path,
                ]

                subprocess.run(command, check=True)

                print(f"✅ ダウンロード成功: {artist} - {track}")

                self.downloaded_files[url] = mp3_path

            except subprocess.CalledProcessError as e:
                print(f"❌ ダウンロード失敗: {artist} - {track}. エラー: {e}")
                self.downloaded_files[url] = None
            except Exception as e:
                print(f"⚠️ 警告: ファイル {artist} - {track} の読み込み中にエラーが発生しました: {e}")
            finally:
                time.sleep(1)


    def process_and_update_csvs(self, all_csv_files: List[str]):
        """
        元のCSVファイルを読み込み、ダウンロードしたMP3のパスを追記し、
        元のファイル構造を再現して新しいCSVファイルに書き出す。
        
        * ダウンロードに失敗した/MP3パスがない行はスキップし、CSVに書き込まない。
        """
        print("\n--- 4. CSVファイルの更新と保存 ---")

        # 新しいヘッダーを定義 (既存のヘッダーにMP3_Local_Pathを追加)
        # self.output_headerがNoneでないことを前提
        if self.output_header is None:
            print("エラー: ヘッダー情報がありません。処理をスキップします。")
            return

        updated_header = self.output_header + ["MP3_Local_Path"]

        for input_file_path in all_csv_files:
            current_file_data = [] # このファイルから抽出されたデータ行を格納

            try:
                with open(input_file_path, mode="r", encoding="utf-8") as infile:
                    reader = csv.reader(infile)
                    next(reader) # ヘッダーをスキップ

                    for row in reader:
                        spotify_url = row[3].strip()

                        # ダウンロード結果のキャッシュを参照
                        # 値は (artist, track, mp3_path) のタプル
                        download_info = self.downloaded_files.get(spotify_url)

                        # ★★★ フィルタリングとデータ追記 ★★★
                        # download_info が存在し、かつ mp3_path が有効な値(None/失敗フラグでない)かチェック
                        if download_info:
                            mp3_path = download_info
                            # 新しい行データ: 元の行 + MP3パス
                            current_file_data.append(row + [mp3_path])
                        # else: ダウンロードに失敗した行は current_file_data に追加されずスキップされる

            except Exception as e:
                print(f"⚠️ 警告: ファイル {input_file_path} の読み込み中にエラーが発生しました: {e}")
                continue
            
            
            # フィルタリングの結果、データが残った場合のみ保存
            if not current_file_data:
                print(f"💡 スキップ: ファイル {input_file_path} は、有効なMP3データがないため作成されません。")
                continue
            
            # 2. フォルダ構造の再現とパスの構築 (前のステップで学習したロジックを適用)
            relative_path = os.path.relpath(os.path.dirname(input_file_path), self.input_root)
            output_file_dir = os.path.join(self.output_root, relative_path)
            os.makedirs(output_file_dir, exist_ok=True)

            base_filename = os.path.basename(input_file_path).replace('.csv', '_with_mp3.csv')
            output_file_path = os.path.join(output_file_dir, base_filename)

            # 3. 書き出し
            try:
                with open(output_file_path, mode='w', newline='', encoding='utf-8') as outfile:
                    writer = csv.writer(outfile)
                    writer.writerow(updated_header)
                    writer.writerows(current_file_data)

                print(f"✅ 保存成功: {output_file_path}")
            except Exception as e:
                print(f"エラー: ファイル '{output_file_path}' への書き出し中に問題が発生しました。{e}")



def main():
    downloader = Mp3Downloader()

    # 1. ROOT_INPUTから全てのcsvファイルの名前を返す
    all_csv_files = downloader.find_input_csvs()
    if not all_csv_files: return
    
    # 2. 全てのcsvファイルからSpotifyURLをcollect、重複はなし
    # [spotifyURL, [artist, track]]
    unique_urls = downloader.collect_unique_urls(all_csv_files)
    if not unique_urls: return

    # 3. mp3をダウンロード
    # class内変数のself.downloaded_files[spotifyURL]にmp3の階層が保存される
    downloader.execute_download(unique_urls)

    downloader.process_and_update_csvs(all_csv_files)

if __name__ == "__main__":
    #download_spotify_mp3()
    main()