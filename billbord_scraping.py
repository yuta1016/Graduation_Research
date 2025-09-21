from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import Select # Selectクラスをインポート

"""
ビルボードURL
https://billboard-japan.com/chart_insight/
"""

# Chromeブラウザを起動
driver = webdriver.Chrome()
# Webページにアクセス
driver.get("https://billboard-japan.com/chart_insight/")


# --------------------------------------------------------------------------
def test():
    # 曲名を取得
    text = driver.find_element(By.CLASS_NAME, song_class_name).text
    print(text)
    # artist名を取得
    artist = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-airtist").text
    print(artist)



# ----------------------------------------------------------------------------------------
# webページの曲名とアーチスト名があるタグのclass名を取得
song_class_name = "list-ttl-box-name-main"
artist_class_name = "list-ttl-box-name-airtist"

def scraping_song_artist(song_class, artist_class):
    initial_song_elements = driver.find_elements(By.CLASS_NAME, "list-ttl-box-name-main")
    initial_artist_elements = driver.find_elements(By.CLASS_NAME, "list-ttl-box-name-airtist")

    for i in range(len(initial_song_elements)):
        song_title = initial_song_elements[i].text
        artist_name = initial_artist_elements[i].text
        print(f"曲名: {song_title}, アーティスト: {artist_name}")
    print("----------------------\n")


# --------------------------------------------------------------------------------------------------
# `<select>`要素をIDで取得
# `Select`クラスのインスタンスを作成
years_select_element = driver.find_element(By.ID, "period-week-year")
months_select_element = driver.find_element(By.ID, "period-week-month")
days_select_element = driver.find_element(By.ID, "period-week-day")

select_elements = [years_select_element, months_select_element, days_select_element]

years = ['2013', '2014', '2015', '2016', '2017', '2018', '2019', '2020', '2021', '2022', '2023', '2024', '2025']
months = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12']

# 2次元
# [0][-] yearsが入る、、[1][-] monthsが入る、、[2][-] daysが入る
values_id = []
tantetive_values_id = []

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



# `value`属性が'2015'の<option>を選択
year = 2017
#years_select_obj.select_by_value("2017")
print(f"★{year}年を選択した")

# 選択された値を確認
#print("選択された年:", years_select_obj.first_selected_option.text)

wait = WebDriverWait(driver, timeout=5)


check_input = driver.find_element(By.ID, "update_btn_period")
check_input.click()

wait = WebDriverWait(driver, timeout=5)
wait.until(EC.presence_of_element_located((By.CLASS_NAME, "list-ttl-box-name-main")))

# 曲名を取得
text = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-main").text
print(text)

# artist名を取得
artist = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-airtist").text
print(artist)


# ブラウザを終了
driver.quit()