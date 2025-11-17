import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import csv
import time
import os
from dotenv import load_dotenv
load_dotenv()


# --- 設定 ---
CLIENT_ID = os.getenv("CLIENT_ID")
CLIENT_SECRET = os.getenv("CLIENT_SECRET")

INPUT_ROOT = './test_input/'
OUTPUT_ROOT = './test_output/'

# -----------

def setup_spotify_client(client_id, client_secret):
    if client_id == "YOUR_CLIENT_ID" or client_secret == "YOUR_CLIENT_SECRET":
        print("エラー: CLIENT_ID または CLIENT_SECRET が設定されていません。")
        return None
    
    client_credentials = SpotifyClientCredentials(
        client_id=client_id,
        client_secret=client_secret
    )
    return spotipy.Spotify(client_credentials_manager=client_credentials)


def find_input_csv_files(root_dir):
    """指定されたルートディレクトリ以下のすべてのCSVファイルを見つけてパスのリストを返す"""
    csv_files = []
    print(f"ディレクトリ '{root_dir}' 以下からCSVファイルを検索中...")
    
    # 再帰的にファイルを検索
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith('.csv'):
                # 相対パスを取得
                full_path = os.path.join(dirpath, filename)
                csv_files.append(full_path)
    
    return csv_files



def search_and_get_url(sp, artist_name, track_name, target_artist_id):
    """
    曲名、アーティスト名で検索し、事前に取得したアーティストIDと一致するトラックのURLを返す。
    """
    # トラック名のみで検索クエリを作成
    query = f"track:{track_name} artist:{artist_name}"

    try:
        print(f"デバッグ: トラック検索クエリ: '{query}' を実行中...")
        results = sp.search(q=query, type='track', limit=5)
        found_url = None
        time.sleep(5)

        # 検索結果をループして、アーティストIDが一致するものを探す
        for track in results['tracks']['items']:
            result_track_name = track['name'].strip()

            # 検索結果のアーティストIDを取得
            # トラックが複数のアーティストを持つ場合もあるが、ここでは最初のIDを取得
            result_artist_id = track['artists'][0]['id'] if track['artists'] else None
            
            # --- 照合ロジック ---
            # 1. 曲名が完全一致すること (小文字比較で表記ゆれを吸収)
            # 2. アーティストIDが事前に検索したIDと一致すること
            if (result_track_name.lower() == track_name.lower()) and \
               (result_artist_id == target_artist_id):
                
                # 完全に一致した場合、URLを保存してループを抜ける
                found_url = track['external_urls']['spotify']
                result_song_popularity = track['popularity']
                break
            else:
                # デバッグ: 一致しなかった場合の情報を表示
                debug_artist_name = track['artists'][0]['name'] if track['artists'] else "N/A"
                print(f"デバッグ: 照合失敗: 曲名='{result_track_name}', アーティスト名='{debug_artist_name}' (ID: {result_artist_id})")

        return found_url, result_song_popularity
    except Exception as e:
        print(f"エラーが発生しました (曲名: {track_name}): {e}")
        return None, None
    finally:
        time.sleep(2) # APIリクエスト間隔を空ける



def search_artist_id(sp, artist_name):
    """
    アーティスト名で検索し、SpotifyのアーティストIDを返す
    (表記ゆれに対応するため、検索結果の最初のIDを使う)
    """
    query = f"{artist_name}"
    try:
        results = sp.search(q=query, type='artist', limit=4)
        time.sleep(5)

        if results['artists']['items']:
            artist_id = results['artists']['items'][0]['id']
            spotify_name = results['artists']['items'][0]['name']
            artist_popularity = results['artists']['items'][0]['popularity']

            print(f"アーティストID取得: 入力名='{artist_name}' -> Spotify名='{spotify_name}', ID='{artist_id}'")
            return artist_id, artist_popularity
        else:
            return None, None
    except Exception as e:
        print(f"エラー: アーティストID検索中に問題が発生しました ({artist_name}): {e}")
        return None, None
    finally:
        time.sleep(2) # APIリクエスト間隔を空ける



def process_single_csv(sp, input_file_path, input_root, output_root):
    """
    単一のCSVファイルを読み込み、SpotifyAPIを使って検索を実行し、結果を対応する
    出力フォルダ構造に書き込む。
    """
    print(f"\n=======================================================")
    print(f"📄 処理開始: {input_file_path}")
    print(f"=======================================================")

    # 1. 入力CSVを読み込み-------------------------------------------------------------------------------------------------------
    try:
        with open(input_file_path, mode='r', encoding='utf-8') as infile:
            reader = csv.reader(infile)
            header = next(reader)
            songs_to_search = list(reader)
    except Exception as e:
        print(f"エラー: ファイル '{input_file_path}' の読み込み中に問題が発生しました。{e}")
        return


    # 2. アーティストIDの事前取得 (artist_idsキャッシュはファイルごとにリセットされるが、ここでは簡単のためこのまま)--------------------
    artist_ids = {}
    artist_popularity = {}
    unique_artists = sorted(list(set(row[2].strip() for row in songs_to_search if len(row) >= 2)))#'row[1]'が曲名, 'row[2]'がアーティスト名

    print(f"\n--- 1. アーティストIDの事前取得 ({len(unique_artists)}名) ---")
    for artist in unique_artists:
        artist_id, artist_popularity = search_artist_id(sp, artist)

        if artist_id:
            artist_ids[artist] = artist_id, artist_popularity
        else:
            artist_ids[artist] = None, None # 見つからない場合も格納


    # 3. 楽曲のURL検索---------------------------------------------------------------------------------------
    results_data = []
    total_songs = len(songs_to_search)
    print(f"\n--- 2. 楽曲のURL検索 ({total_songs}件) ---")
    
    for index, row in enumerate(songs_to_search):
        if len(row) < 3: # アーティスト名と曲名があるか確認
            results_data.append([row[0].strip() if row else "N/A", "N/A", "スキップされました（データ不足）"])
            continue

        track = row[1].strip()
        artist = row[2].strip()
        target_id = artist_ids.get(artist)[0]
        artist_popularity = artist_ids.get(artist)[1]
        spotify_url = None

        if target_id:
            spotify_url, song_popularity = search_and_get_url(sp, artist, track, target_id)# IDを使ってトラックを検索
        
        if spotify_url is None:
            continue  # URLが見つからなかった場合、次の曲へ

        results_data.append([
            artist,
            track,
            spotify_url,
            artist_popularity,
            song_popularity
        ])

        print(f"{index + 1}/{total_songs} - {artist} - {track} - {artist_popularity} - {song_popularity}: {'取得完了' if spotify_url else '見つからず'}")


    # 4. 結果をCSVファイルに書き込み------------------------------------------------------------------------------------------------
    # フォルダ構造を計算: input_rootからの相対パスを取得
    relative_path = os.path.relpath(os.path.dirname(input_file_path), input_root)
    
    # 出力ディレクトリパスを構築
    output_dir = os.path.join(output_root, relative_path)
    os.makedirs(output_dir, exist_ok=True)
    
    # 出力ファイル名を構築 (元のファイル名に "_spotify_url" を追加)
    base_filename = os.path.basename(input_file_path).replace('.csv', '_spotify_info.csv')
    output_file_path = os.path.join(output_dir, base_filename)
    
    output_header = ['アーティスト名', '曲名', 'Spotify URL', 'アーティスト人気度', '曲人気度']
    try:
        with open(output_file_path, mode='w', newline='', encoding='utf-8') as outfile:
            writer = csv.writer(outfile)
            writer.writerow(output_header)
            writer.writerows(results_data)
        
        print(f"✅ 成功: 結果がファイル '{output_file_path}' に保存されました。")
    except Exception as e:
        print(f"エラー: 結果のCSVファイル書き出し中に問題が発生しました。{e}")



def main():
    sp = setup_spotify_client(CLIENT_ID, CLIENT_SECRET)
    if sp is None: return

    all_csv_files = find_input_csv_files(INPUT_ROOT)
    if not all_csv_files:
        print("エラー: 処理対象のCSVファイルが見つかりませんでした。")
        return

    print(f"処理対象ファイル数: {len(all_csv_files)}件")
    print("-----------------------------------------------------------")

    # 各CSVファイルに対して処理を実行
    for csv_file in all_csv_files:
        # 単一CSVファイルを処理する新しい関数を呼び出し
        process_single_csv(sp, csv_file, INPUT_ROOT, OUTPUT_ROOT)

    print("\n=======================================")
    print("🎉 すべてのファイルの処理が完了しました。")
    print("=======================================")



if __name__ == "__main__":
    main()