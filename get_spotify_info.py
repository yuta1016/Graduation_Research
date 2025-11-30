import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import csv
import time
import os
from dotenv import load_dotenv
load_dotenv()

import send_mail

# --- 設定 ---
CLIENT_ID = os.getenv("CLIENT_ID")
CLIENT_SECRET = os.getenv("CLIENT_SECRET")

# INPUT_ROOT = './test_input/'
# OUTPUT_ROOT = './test_output/'
INPUT_ROOT = './filtered_billboard_charts/'
OUTPUT_ROOT = './exact_match_csv_of_spotify_info/'

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
        result_song_popularity = None

        # 検索結果をループして、アーティストIDが一致するものを探す
        for track in results['tracks']['items']:
            result_track_name = track['name'].strip()
            # 検索結果のアーティストIDを取得
            # トラックが複数のアーティストを持つ場合もあるが、ここでは最初のIDを取得
            result_artist_id = track['artists'][0]['id'] if track['artists'] else None
            
            input_name_lower = track_name.lower()
            result_name_lower = result_track_name.lower()

            #name_match = input_name_lower in result_name_lower
            name_match = (input_name_lower == result_name_lower)
            id_match = (result_artist_id == target_artist_id)

            # --- 照合ロジック ---
            if name_match and id_match:
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



def process_single_csv(sp, input_file_path, input_root, output_root, artist_cache, track_url_cache):
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


    # 2. 楽曲のURL検索---------------------------------------------------------------------------------------
    results_data = []
    total_songs = len(songs_to_search)
    print(f"\n--- 2. 楽曲のURL検索 ({total_songs}件) ---")
    
    for index, row in enumerate(songs_to_search):
        if len(row) < 3: # アーティスト名と曲名があるか確認
            results_data.append([row[0].strip() if row else "N/A", "N/A", "スキップされました（データ不足）"])
            continue

        track = row[1].strip()
        artist = row[2].strip()
        score = row[3]

        # target_info = artist_cache.get(artist, (None, None))
        # target_id = target_info[0]
        # artist_popularity = target_info[1]

        
        # --------------------------------------------------------
        # ★★★ 改善点: キャッシュを使った効率的な検索 ★★★
        # --------------------------------------------------------
        song_key = (artist, track)
        
        # 1. まずキャッシュを確認
        if song_key in track_url_cache:
            spotify_url, song_popularity = track_url_cache[song_key]
            # print(f"キャッシュヒット: {artist} - {track}") # デバッグ用
        else:
            # 2. キャッシュになければAPI検索
            target_info = artist_cache.get(artist, (None, None))
            target_id = target_info[0]
            artist_popularity = target_info[1] # ここで取得しておく

            spotify_url = None
            song_popularity = None

            if target_id:
                spotify_url, song_popularity = search_and_get_url(sp, artist, track, target_id)


            if spotify_url is None:
                continue  # URLが見つからなかった場合、次の曲へ
            
            # 3. 結果をキャッシュに保存 (Noneでも保存して再検索を防ぐ)
            track_url_cache[song_key] = (spotify_url, song_popularity)
            
            # APIを叩いた時だけ進捗を表示
            artist_pop_display = artist_popularity if artist_popularity else "N/A"
            song_pop_display = song_popularity if song_popularity else "N/A"
            print(f"{index + 1}/{total_songs} - {artist} - {track}: {'取得完了' if spotify_url else '見つからず'}")

        # --------------------------------------------------------

        # アーティスト人気度は artist_cache から再取得（ループ内で上書きされている可能性があるため）
        artist_popularity = artist_cache.get(artist, (None, None))[1]

        results_data.append([
            artist,
            track,
            score,
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
    
    output_header = ['アーティスト名', '曲名', '順位', 'Spotify URL', 'アーティスト人気度', 'track人気度']
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

    # ========================================================
    # ★★★ 修正箇所 1: 全ファイルからユニークアーティストを収集 ★★★
    # ========================================================
    all_unique_artists = set()
    
    # 全CSVファイルを読み込み、すべてのアーティスト名を収集
    print("--- 全ファイルからアーティスト名を収集中 ---")
    for csv_file in all_csv_files:
        try:
            with open(csv_file, mode='r', encoding='utf-8') as infile:
                reader = csv.reader(infile)
                header = next(reader)
                # artists in row[2]
                all_unique_artists.update(row[2].strip() for row in reader if len(row) > 2)
        except Exception as e:
            print(f"警告: ファイル {csv_file} のアーティスト収集に失敗しました: {e}")
            continue

    # ========================================================
    # ★★★ 修正箇所 2: 統合されたリストを使ってIDを一度だけ取得 ★★★
    # ========================================================
    artist_cache = {}
    sorted_artists = sorted(list(all_unique_artists))
    
    print(f"\n--- 1. 全アーティストIDの事前取得 ({len(sorted_artists)}名) ---")
    for artist in sorted_artists:
        artist_id, artist_popularity = search_artist_id(sp, artist)
        artist_cache[artist] = (artist_id, artist_popularity) # IDと人気度をタプルで格納


    # 各CSVファイルに対して処理を実行
    # キー: (artist, track), 値: (spotify_url, song_popularity)
    track_url_cache = {}
    for csv_file in all_csv_files:
        # 単一CSVファイルを処理する新しい関数を呼び出し
        process_single_csv(sp, csv_file, INPUT_ROOT, OUTPUT_ROOT, artist_cache, track_url_cache)

    send_mail.prosess_mail("mp3のダウンロードとcsvの更新が完了しました!!!get_spotify_info.pyからのメール")

    print("\n=======================================")
    print("🎉 すべてのファイルの処理が完了しました。")
    print("=======================================")



if __name__ == "__main__":
    main()