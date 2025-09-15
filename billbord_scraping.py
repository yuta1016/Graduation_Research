from selenium import webdriver
"""
ビルボードURL
https://billboard-japan.com/chart_insight/
"""

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import Select # Selectクラスをインポート


# Chromeブラウザを起動
driver = webdriver.Chrome()
# Webページにアクセス
driver.get("https://billboard-japan.com/chart_insight/")


# rect = driver.find_element(By.ID, "period-week-year").rect
# print("動的要素のテキスト:", rect)

# text = driver.find_element(By.ID, "period-week-year").text
# print("動的要素のテキスト:", text)

# 曲名を取得
text = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-main").text
print(text)

# artist名を取得
artist = driver.find_element(By.CLASS_NAME, "list-ttl-box-name-airtist").text
print(artist)

is_enabled_button = driver.find_element(By.ID, "period-week-year").is_enabled()
assert is_enabled_button == True

# `<select>`要素をIDで取得
# `Select`クラスのインスタンスを作成
select_element = driver.find_element(By.ID, "period-week-year")
select_obj = Select(select_element)
# `value`属性が'2015'の<option>を選択
year = 2017
select_obj.select_by_value("2017")
print(f"★{year}年を選択した")

# 選択された値を確認
print("選択された年:", select_obj.first_selected_option.text)

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


# isSelected
#is_selected_check = driver.find_element(By.ID, "period-week-year").is_selected()
#assert is_selected_check == True

# ブラウザを終了
driver.quit()