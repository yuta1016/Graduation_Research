from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import Select
import time
import pandas as pd

"""
ビルボードURL
https://billboard-japan.com/chart_insight/
"""

# Chromeブラウザを起動
driver = webdriver.Chrome()
# Webページにアクセス
driver.get("https://billboard-japan.com/chart_insight/")
# ページの読み込みを待つための変数
wait = WebDriverWait(driver, timeout=5)



# --------------------------------------------------------------------------
def test_retrive_songartist_onlyone():
    wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main"))) # 読み込まれるまで待つ

    # 曲名を取得
    text = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-main").text
    print(text)
    # artist名を取得
    artist = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-airtist").text
    print(artist)
    # rankを取得
    rank = driver.find_element(By.CLASS_NAME, "rank").text
    print(rank)


#----------------------------------------------------------------------------------------
def create_csvfile_including_song_artist(df_song_artist_rank, csv_name):
    df_song_artist_rank.to_csv(csv_name, index=False, encoding='utf-8-sig')


#--------------------------------------------------------------------------------------
def create_dataframe_to_save_song_aritist(data_song_artist_rank, csv_name):
    df = pd.Dataframe(data_song_artist_rank)
    print(df)
    create_csvfile_including_song_artist(df, csv_name)


# ----------------------------------------------------------------------------------------
# webページの曲名とアーチスト名があるタグのclass名を取得
song_class_name = "list-ttl-box-name-main"
artist_class_name = "list-ttl-box-name-airtist"
rank_class_name = "ul.ranking-list-cmn li.list-chart span.rank"


def scraping_song_and_artist(year, month, day):
    data_song_artist_rank = []

    song_elements = driver.find_elements(By.CLASS_NAME, song_class_name)
    artist_elements = driver.find_elements(By.CLASS_NAME, artist_class_name)
    rank_elements = driver.find_elements(By.CLASS_NAME, rank_class_name)

    for i in range(len(song_elements)):
        song_title = song_elements[i].text
        artist_name = artist_elements[i].text
        rank = rank_elements[i].text
        print(f"順位: {rank}, 曲名: {song_title}, アーティスト: {artist_name}")
        
        data_song_artist_rank.append({
            "rank": rank,
            "song_title": song_title,
            "artist_name": artist_name
        })

    df = pd.DataFrame(data_song_artist_rank)
    print(df)
    csv_name= f"{year}_{month}_{day}.csv"
    print("----------------------\n")



# --------------------------------------------------------------------------------------------------
# `<select>`要素をIDで取得
# `Select`クラスのインスタンスを作成
years_class_name = "period-week-year"
months_class_name = "period-week-month"
days_class_name = "period-week-day"

years_select_element = driver.find_element(By.ID, years_class_name)
months_select_element = driver.find_element(By.ID, months_class_name)
days_select_element = driver.find_element(By.ID, days_class_name)

years_select_obj = Select(years_select_element)
months_select_obj = Select(months_select_element)
days_select_obj = Select(days_select_element)

years = ['2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024', '2025']
months = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '1', '11', '12']

def retrive_info_with_datacomponents():
    for year in years:
        wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main"))) # 読み込まれるまで待つ
        years_select_obj.select_by_value(year)

        for month in months:
            wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main"))) # 読み込まれるまで待つ
            months_select_obj.select_by_value(month)

            for day_option in days_select_obj.options:
                wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main"))) # 読み込まれるまで待つ
                day = day_option.get_attribute('value')
                days_select_obj.select_by_value(day)
                print(f"{year}年{month}月{day}日")

                # 検索ボタンをクリック
                check_input = driver.find_element(By.ID, "update_btn_period")
                check_input.click()
                wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main"))) # 読み込まれるまで待つ

                #test_retrive_songartist_onlyone()
                scraping_song_and_artist(year, month,  day)
                time.sleep(2)


#-------------------------------------------------------------------------------------------------------
# 2次元
# [0][-] yearsが入る、、[1][-] monthsが入る、、[2][-] daysが入る
values_id = []
tantetive_values_id = []
select_elements = [years_select_element, months_select_element, days_select_element]

# Webサイトのclass要素から抽出する
def retrive_info_song_and_artist_from_class():
    for element in select_elements:
        select_obj = Select(element)
        print(f"〇{element.get_attribute("id")}のオプション")

        for option in select_obj.options:
            print(f"テキスト: {option.text}, 値: {option.get_attribute('value')}")
            tantetive_values_id.append(option.get_attribute("value"))

        values_id.append(tantetive_values_id)
        tantetive_values_id = []

        print("--------------------------------------\n")

        print(values_id)


# -------------------------------------------------------------------------------
def kind_of_test():
    year = "2014"
    years_select_obj.select_by_value(year)
    print(f"★ {year}年を選択した")

    month = "2"
    months_select_obj.select_by_value(month)
    print(f"★ {month}月を選択した")

    #day = days_select_obj.get_attribute('value')
    #days_select_obj.select_by_value(day)
    for option in days_select_obj.options:
        print(f"テキスト: {option.text}, 値: {option.get_attribute('value')}")
        tantetive_values_id.append(option.get_attribute("value"))


    check_input = driver.find_element(By.ID, "update_btn_period")
    check_input.click()

    wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main"))) # 読み込まれるまで待つ

    # 曲名を取得
    text = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-main").text
    print(text)

    # artist名を取得
    artist = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-airtist").text
    print(artist)




retrive_info_with_datacomponents()

# ブラウザを終了
driver.quit()