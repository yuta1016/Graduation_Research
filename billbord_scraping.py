import os
import time
import pandas as pd
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support.ui import Select
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.remote.remote_connection import RemoteConnection

"""
https://billboard-japan.com/charts/detail?a=hot100
"""

# --- 定数（グローバル定数は大文字スネークケースが一般的）---
BASE_URL = "https://billboard-japan.com/charts/detail?a=hot100"
WAIT_TIMEOUT = 10

# --- song artist rankの対応しているCSS ---
SONG_CLASS = "musuc_title"
ARTIST_CLASS = "artist_name"
RANK_CLASS = "rank"

# --- セレクトボタン、それぞれのID
YEARS_ID = "year"
MONTHS_ID = "month"
DAYS_ID = "day"

# ---検索ボタンのID---
UPDATE_BUTTON_ID = "submit"

OUTPUT_FOLDER = "billboard_charts"
#OUTPUT_FOLDER = "test_csv"

# --- 処理対象の年月設定 ---
TARGET_YEARS = ['2018', '2019', '2020', '2021', '2022','2023', '2024', '2025']
#TARGET_YEARS = ['2022', '2023', '2024', '2025']
# 正しいリスト：months = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']
TARGET_MONTHS = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12']


# --------------------------------------------------------------------------
## 🚀 クライアント初期化
# --------------------------------------------------------------------------
def initialize_driver(url, timeout):
    """Chrome WebDriverを初期化し、指定されたURLにアクセスする。"""
    print(f"Webドライバを起動し、{url}にアクセスします...")

    # ★★★ 修正箇所: タイムアウトオプションを設定 ★★★
    options = Options()
    # コマンド実行のタイムアウトを延ばす (例: 300秒)
    options.page_load_strategy = 'normal' # ページ全体がロードされるのを待つ
    
    driver = webdriver.Chrome(options=options)

    # ★★★ 修正箇所: コマンドタイムアウトを延長 ★★★
    try:
        # RemoteConnectionの内部タイムアウトを300秒に延長
        # 既存のcommand_executor._urlを使用して新しいRemoteConnectionを作成し、上書きする
        new_executor = RemoteConnection(driver.command_executor._url, keep_alive=True)
        new_executor.set_timeout(300) # 300秒 (5分) に設定
        driver.command_executor = new_executor
        print("✅ WebDriverコマンドタイムアウトを300秒に延長しました。")
    except Exception as e:
        print(f"警告: コマンドタイムアウト延長に失敗しました: {e}")


    # ページロードタイムアウトを延ばす (例: 180秒)
    driver.set_page_load_timeout(600)

    driver.get(url)
    wait = WebDriverWait(driver, timeout=timeout)
    return driver, wait

# --------------------------------------------------------------------------
## 💾 データ保存処理
# --------------------------------------------------------------------------
def create_csv_file(data_list, csv_name, year, month):
    """データをDataFrameに変換し、年別フォルダにCSVとして保存する。"""
    df = pd.DataFrame(data_list)
    print("--- 保存データ ---")
    print(df)

    # 年ごと1～12月のフォルダを作成
    output_dir = os.path.join(OUTPUT_FOLDER, str(year))
    os.makedirs(output_dir, exist_ok=True)
    
    # ファイルパスを結合 (例: billboard_charts/2022/2022_1_16.csv)
    file_path = os.path.join(output_dir, csv_name)

    # utf-8-sig は Excel で開く際に文字化けを防ぐのに役立ちます
    df.to_csv(file_path, index=False, encoding="utf-8-sig")
    print(f"✅ CSVファイル '{file_path}' を作成しました。\n")


# --------------------------------------------------------------------------
## 🎼 スクレイピング処理
# --------------------------------------------------------------------------
def scrape_current_chart(driver, year, month, day):
    """現在のWebページから楽曲情報（曲名、アーティスト名、順位）を取得する。"""
    data_song_artist_rank = []

    # データが読み込まれるまで待機
    wait = WebDriverWait(driver, WAIT_TIMEOUT)
    wait.until(EC.presence_of_element_located((By.CLASS_NAME, SONG_CLASS)))
    
    # 全ての楽曲エレメントを取得
    song_elements = driver.find_elements(By.CLASS_NAME, SONG_CLASS)
    artist_elements = driver.find_elements(By.CLASS_NAME, ARTIST_CLASS)
    
    # 曲数（要素の数）は一致すると仮定し、短い方に合わせる
    num_songs = min(len(song_elements), len(artist_elements))

    print(f"取得中: {year}年{month}月{day}日（{num_songs}件）")

    for i in range(num_songs):
        song_title = song_elements[i].text.strip()
        artist_name = artist_elements[i].text.strip()
        rank = i + 1 # 順位はリストのインデックス+1とする
        score = 100 - i
        
        # print(f"順位: {rank}, 曲名: {song_title}, アーティスト: {artist_name}")
        
        data_song_artist_rank.append({
            "rank": rank,
            "song_title": song_title,
            "artist_name": artist_name,
            "score" : score
        })

    csv_name = f"{year}_{month}_{day}.csv"
    create_csv_file(data_song_artist_rank, csv_name, year, month)
    time.sleep(2) # サーバーへの負荷軽減のための待機


# --------------------------------------------------------------------------
## ✨ ヘルパー関数：ドロップダウン選択
# --------------------------------------------------------------------------
def reacquire_and_select(driver, element_id, value, wait):
    """
    指定されたIDのSelect要素を再取得し、指定された値を選択する。
    Stale Element Reference Errorを防ぐため、毎回要素を再取得する。
    """
    # 1. 要素を再取得
    element = driver.find_element(By.ID, element_id)
    select_obj = Select(element)
    
    # 2. 値を選択
    select_obj.select_by_value(value)
    
    # 3. DOMの更新を待つ
    wait.until(EC.presence_of_element_located((By.CLASS_NAME, SONG_CLASS)))


# --------------------------------------------------------------------------
## 📅 メインのデータ収集ロジック
# --------------------------------------------------------------------------
def retrieve_info_from_japanchart(driver, wait):
    """
    year, month, dayのセレクトボックスから対象年月日を選択し、
    各日のチャートデータをスクレイピングしてCSVに保存する。
    """

    for year in TARGET_YEARS:
        print(f"\n======== {year}年のデータを処理開始 ========")
        # 年、月、日、検索ボタンなどのID取得は動的Web要素のため、検索ボタンを押すたびに再取得が必要
        reacquire_and_select(driver, YEARS_ID, year, wait)

        for month in TARGET_MONTHS:
            print(f"\n======== {month}月のデータを処理開始 ========")
            reacquire_and_select(driver, MONTHS_ID, month, wait)
            
            # 年月でvalueが変わるためここでvalue取得
            days_value_list = []
            days_select_obj = Select(driver.find_element(By.ID, DAYS_ID))
            for option in days_select_obj.options:
                days_value_list.append(option.get_attribute('value')) # 値（文字列）をコピーしてリストに格納

            # 取得した日付オプションを全て試す
            for day in days_value_list:
                print(f"\n======== {day}日のデータを処理開始 ========")
                reacquire_and_select(driver, DAYS_ID, day, wait)

                slice_day = day[-2:]
                print(f"{year}年{month}月{slice_day}日")
                
                # 検索ボタンをクリックしてチャートを更新
                check_input = driver.find_element(By.ID, UPDATE_BUTTON_ID)
                check_input.click()
                
                # チャートの更新を待機
                wait.until(EC.presence_of_element_located((By.CLASS_NAME, SONG_CLASS)))
                print("5秒待機中..........................")
                time.sleep(5)

                
                # スクレイピング実行
                scrape_current_chart(driver, year, month, slice_day)

# --------------------------------------------------------------------------
## 🖥️ メイン関数
# --------------------------------------------------------------------------
def main():
    """プログラムのエントリーポイントとして機能するメイン関数。"""
    driver, wait = None, None
    try:
        # 1. 初期化とアクセス
        driver, wait = initialize_driver(BASE_URL, WAIT_TIMEOUT)

        # 2. メインのデータ収集ロジック
        retrieve_info_from_japanchart(driver, wait)
        
        print("\n==================================")
        print("🎉 全期間のデータ収集が完了しました。")
        print("==================================")

    except Exception as e:
        print(f"\n致命的なエラーが発生しました: {e}")
        # デバッグのためにエラーメッセージを詳細に表示する
        from traceback import print_exc
        print_exc()
    finally:
        # 3. ブラウザを終了 (エラー時でも実行されるように finally に配置)
        if driver:
            driver.quit()

if __name__ == "__main__":
    main()