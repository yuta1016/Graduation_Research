import pandas as pd
import numpy as np
from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import balanced_accuracy_score
import itertools

# ==========================================
# 1. 設定 (CONFIGURATION)
# ==========================================

# --- ファイルパス設定 ---
PATH_TRAIN = 'train.csv'
PATH_VAL = 'val.csv'
PATH_TEST = 'test.csv'
PATH_COMPLEXITY = 'complexity.csv'
PATH_MFCC = 'mfcc.csv'

# --- ターゲット設定 ---
# 予測対象のカラム名 ('Track_Popularity' または 'Artist_Popularity')
TARGET_COL = 'Track_Popularity'

# --- 特徴量設定 ---
# もう一方の人気度指標を特徴量として含めるか (True/False)
# 例: TargetがTrack_Popularityのとき、Artist_Popularityを特徴量に入れるか
USE_OTHER_POPULARITY = False 

# --- 特徴量リストの定義 (DEFINE) ---
# 1. Main CSVに含まれる基本統計量
FEAT_MAIN_STATS = ['Max', 'Mean', 'Std', 'Length', 'Sum', 'Skewness', 'Kurtosis']

# 2. Complexity CSVに含まれる特徴量
FEAT_COMPLEXITY = [
    'Chroma3', 'Chroma4', 'Chroma5', 'Chroma6', 'Chroma7', 'Chroma8',
    'Rhythm1', 'Rhythm2', 'Rhythm3', 'Rhythm4', 'Rhythm5', 'Rhythm6',
    'Timbre1', 'Timbre2', 'Timbre3', 'Timbre4', 'Timbre5', 'Timbre6',
    'ArousalMean', 'ArousalStd'
]

# 3. MFCC CSVに含まれる特徴量 (カラム名が '0' ～ '31' であると仮定)
FEAT_MFCC = [str(i) for i in range(32)]


# ==========================================
# 2. データ読み込み・前処理クラス
# ==========================================

class DataLoader:
    def __init__(self, train_path, val_path, test_path, comp_path, mfcc_path):
        self.train_path = train_path
        self.val_path = val_path
        self.test_path = test_path
        self.comp_path = comp_path
        self.mfcc_path = mfcc_path

    def load_and_merge(self):
        # メインデータの読み込み
        df_train = pd.read_csv(""+self.train_path)
        df_val = pd.read_csv(self.val_path)
        df_test = pd.read_csv(self.test_path)

        # 特徴量データの読み込み
        df_comp = pd.read_csv(self.comp_path)
        df_mfcc = pd.read_csv(self.mfcc_path)

        # URLをキーにして結合 (inner joinでマッチするものだけ残す)
        # 必要であれば how='left' に変更してください
        def merge_feats(base_df):
            tmp = pd.merge(base_df, df_comp, on='URL', how='inner', suffixes=('', '_comp'))
            # MP3_Pathなどが重複する場合のsuffix処理を考慮しつつマージ
            tmp = pd.merge(tmp, df_mfcc, on='URL', how='inner', suffixes=('', '_mfcc'))
            return tmp

        train_merged = merge_feats(df_train)
        val_merged = merge_feats(df_val)
        test_merged = merge_feats(df_test)

        return train_merged, val_merged, test_merged

# ==========================================
# 3. 実験実行用関数
# ==========================================

def get_binary_labels(y, threshold):
    """中央値を閾値として二値化 (閾値より大きい=1, 以下=0)"""
    return (y > threshold).astype(int)

def run_svm_experiment(X_train, y_train, X_val, y_val, X_test, y_test, feature_name):
    """
    Validationデータを使ってハイパーパラメータ(C, gamma)を決定し、
    そのパラメータでTestデータを評価する
    """
    # RBFカーネルSVMのためのスケーリング
    scaler = StandardScaler()
    X_train_s = scaler.fit_transform(X_train)
    X_val_s = scaler.transform(X_val)
    X_test_s = scaler.transform(X_test)

    # グリッドサーチの設定
    C_range = [0.1, 1, 10, 100]
    gamma_range = [0.001, 0.01, 0.1, 1, 'scale']
    
    best_ba = -1
    best_params = {}
    best_model = None

    # Validationセットを用いたパラメータ探索
    for C in C_range:
        for gamma in gamma_range:
            clf = SVC(kernel='rbf', C=C, gamma=gamma, class_weight='balanced')
            clf.fit(X_train_s, y_train)
            
            y_pred_val = clf.predict(X_val_s)
            ba = balanced_accuracy_score(y_val, y_pred_val)
            
            if ba > best_ba:
                best_ba = ba
                best_params = {'C': C, 'gamma': gamma}
                best_model = clf

    # 最良モデルでテストデータを評価
    y_pred_test = best_model.predict(X_test_s)
    test_ba = balanced_accuracy_score(y_test, y_pred_test)

    return {
        'Feature': feature_name,
        'Best_C': best_params['C'],
        'Best_Gamma': best_params['Best_Gamma'] if 'Best_Gamma' in best_params else best_params['gamma'],
        'Val_BA': best_ba,
        'Test_BA': test_ba
    }

# ==========================================
# 4. メイン処理
# ==========================================

def main():
    # データのロードと結合
    loader = DataLoader(PATH_TRAIN, PATH_VAL, PATH_TEST, PATH_COMPLEXITY, PATH_MFCC)
    train_df, val_df, test_df = loader.load_and_merge()

    print(f"Data Loaded. Train: {train_df.shape}, Val: {val_df.shape}, Test: {test_df.shape}")

    # ターゲットラベルの準備
    # Trainデータの中央値を基準にする
    median_val = train_df[TARGET_COL].median()
    print(f"Target: {TARGET_COL}, Median Threshold: {median_val}")

    y_train = get_binary_labels(train_df[TARGET_COL], median_val)
    y_val = get_binary_labels(val_df[TARGET_COL], median_val)
    y_test = get_binary_labels(test_df[TARGET_COL], median_val)

    # 「他方の人気度」を使う場合の処理
    other_pop_col = 'Artist_Popularity' if TARGET_COL == 'Track_Popularity' else 'Track_Popularity'
    extra_features = [other_pop_col] if USE_OTHER_POPULARITY else []

    results = []

    # ---------------------------------------------------------
    # Experiment 1: Prediction using each single complexity feature
    # ---------------------------------------------------------
    print("\n--- Experiment 1: Single Complexity Feature ---")
    # 検証したい単一特徴量リスト (Main Stats + Complexity CSV Features)
    single_features_to_test = FEAT_MAIN_STATS + FEAT_COMPLEXITY
    
    for feat in single_features_to_test:
        # 特徴量の選択 (+ 設定されていれば人気度も)
        cols = [feat] + extra_features
        
        # データフレームから抽出
        X_tr = train_df[cols]
        X_va = val_df[cols]
        X_te = test_df[cols]
        
        res = run_svm_experiment(X_tr, y_train, X_va, y_val, X_te, y_test, f"Single: {feat}")
        results.append(res)
        print(f"Feature: {feat:20} | Test BA: {res['Test_BA']:.4f}")

    # ---------------------------------------------------------
    # Experiment 2: Prediction using each feature group
    # ---------------------------------------------------------
    print("\n--- Experiment 2: Feature Groups ---")
    feature_groups = {
        'Main_Stats': FEAT_MAIN_STATS,
        'Complexity': FEAT_COMPLEXITY,
        'MFCC': FEAT_MFCC
    }

    for group_name, feats in feature_groups.items():
        cols = feats + extra_features
        X_tr = train_df[cols]
        X_va = val_df[cols]
        X_te = test_df[cols]

        res = run_svm_experiment(X_tr, y_train, X_va, y_val, X_te, y_test, f"Group: {group_name}")
        results.append(res)
        print(f"Group: {group_name:20} | Test BA: {res['Test_BA']:.4f}")

    # ---------------------------------------------------------
    # Experiment 3: Prediction using combined features
    # ---------------------------------------------------------
    print("\n--- Experiment 3: Combined Features ---")
    combinations = {
        'Comp + MFCC': FEAT_COMPLEXITY + FEAT_MFCC,
        'Main + Comp': FEAT_MAIN_STATS + FEAT_COMPLEXITY,
        'All Combined': FEAT_MAIN_STATS + FEAT_COMPLEXITY + FEAT_MFCC
    }

    for combo_name, feats in combinations.items():
        cols = feats + extra_features
        X_tr = train_df[cols]
        X_va = val_df[cols]
        X_te = test_df[cols]

        res = run_svm_experiment(X_tr, y_train, X_va, y_val, X_te, y_test, f"Combo: {combo_name}")
        results.append(res)
        print(f"Combo: {combo_name:20} | Test BA: {res['Test_BA']:.4f}")

    # 結果の保存
    res_df = pd.DataFrame(results)
    res_df.to_csv('svm_experiment_results.csv', index=False)
    print("\nAll experiments finished. Results saved to 'svm_experiment_results.csv'.")

if __name__ == "__main__":
    main()