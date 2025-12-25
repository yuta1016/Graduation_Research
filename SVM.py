import pandas as pd
import numpy as np
from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import balanced_accuracy_score
import send_mail

# ==========================================
# 1. 設定 (CONFIGURATION)
# ==========================================

# --- ファイルパス設定 ---
PATH_TRAIN = './billboard_futures_info_by_age/train_2008_2017.csv'
PATH_VAL = './billboard_futures_info_by_age/val_2018_2021.csv'
PATH_TEST = './billboard_futures_info_by_age/test_2022_2025.csv'

PATH_COMPLEXITY = './features_complexity/2008_2025_complexity.csv'
PATH_MFCC = './features_mfcc/2008_2025_mfcc.csv'

# --- ターゲット設定 (8つの人気度指標) ---
TARGET_COLS = ["Debut_Score", "Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis"]

# --- 特徴量の利用設定 (ロジック追加部分) ---
# 他の人気度指標を特徴量として含めるかどうかのフラグ
# True:  現在のターゲット以外の7つの指標を特徴量に含める
# False: 音響特徴量(Complexity/MFCC)のみを使用する
INCLUDE_OTHER_TARGETS_AS_FEATURES = False

# --- 特徴量グループの定義 ---
FEAT_COMPLEXITY = [
    'Chroma3', 'Chroma4', 'Chroma5', 'Chroma6', 'Chroma7', 'Chroma8',
    'Rhythm1', 'Rhythm2', 'Rhythm3', 'Rhythm4', 'Rhythm5', 'Rhythm6',
    'Timbre1', 'Timbre2', 'Timbre3', 'Timbre4', 'Timbre5', 'Timbre6',
    'ArousalMean', 'ArousalStd'
]
FEAT_MFCC = [str(i) for i in range(32)]
SPOTIFY_POPULARITY = ['Artist_Popularity', "Track_Popularity"]

# ==========================================
# 2. データロード・処理関数
# ==========================================

class DataLoader:
    def __init__(self, train_path, val_path, test_path, comp_path, mfcc_path):
        self.paths = [train_path, val_path, test_path]
        self.comp_path = comp_path
        self.mfcc_path = mfcc_path

    def load_and_merge(self):
        df_comp = pd.read_csv(self.comp_path)
        df_mfcc = pd.read_csv(self.mfcc_path)

        def merge_logic(base_path):
            base_df = pd.read_csv(base_path)
            tmp = pd.merge(base_df, df_comp, on='URL', how='inner', suffixes=('', '_comp'))
            tmp = pd.merge(tmp, df_mfcc, on='URL', how='inner', suffixes=('', '_mfcc'))
            return tmp

        return [merge_logic(p) for p in self.paths]


def run_svm_logic(X_train, y_train, X_val, y_val, X_test, y_test):
    # 数値データのみを抽出 (念のため)
    X_train = X_train.select_dtypes(include=[np.number])
    X_val = X_val.select_dtypes(include=[np.number])
    X_test = X_test.select_dtypes(include=[np.number])

    scaler = StandardScaler()
    X_train_s = scaler.fit_transform(X_train)
    X_val_s = scaler.transform(X_val)
    X_test_s = scaler.transform(X_test)

    C_range = [0.1, 1, 10, 100, 1000]
    gamma_range = [0.0001, 0.001, 0.01, 0.1, 1, 'scale']
    
    best_ba_val = -1
    best_model = None

    for C in C_range:
        for gamma in gamma_range:
            clf = SVC(kernel='rbf', C=C, gamma=gamma, class_weight='balanced')
            clf.fit(X_train_s, y_train)
            val_preds = clf.predict(X_val_s)
            ba_val = balanced_accuracy_score(y_val, val_preds)
            
            if ba_val > best_ba_val:
                best_ba_val = ba_val
                best_model = clf

    test_preds = best_model.predict(X_test_s)
    return balanced_accuracy_score(y_test, test_preds)

# ==========================================
# 3. メイン処理
# ==========================================

def main():
    loader = DataLoader(PATH_TRAIN, PATH_VAL, PATH_TEST, PATH_COMPLEXITY, PATH_MFCC)
    train_df, val_df, test_df = loader.load_and_merge()
    
    all_results = []
    mode_str = "with_PopFeatures" if INCLUDE_OTHER_TARGETS_AS_FEATURES else "audio_only"
    print(f"Running Mode: {mode_str}")

    for target in TARGET_COLS:
        print(f"\nTargeting Popularity Metric: [{target}]")
        
        # 中央値で二値化 (境界はTraining set基準)
        """
        print(y_train.value_counts())
            Max
            0    1653
            1    1594
            Name: count, dtype: int64"""
        median_val = train_df[target].median()
        y_train = (train_df[target] > median_val).astype(int)
        y_val = (val_df[target] > median_val).astype(int)
        y_test = (test_df[target] > median_val).astype(int)

        # print(y_train.head(10))
        # exit()

        # 特徴量リストの作成
        # 現在のターゲット以外の人気度指標を特定
        other_pop_metrics = [t for t in TARGET_COLS if t != target]
        
        # 実験設定の定義
        experiments = {
            #"Foe example" : SPOTIFY_POPULARITY,
            "Exp2: Group_Complexity": FEAT_COMPLEXITY,
            "Exp2: Group_MFCC": FEAT_MFCC,
            "Exp3: Combined_All": FEAT_COMPLEXITY + FEAT_MFCC,
            "Exp4: Spotify_Popularity": FEAT_COMPLEXITY + FEAT_MFCC + SPOTIFY_POPULARITY
        }

        for exp_name, base_features in experiments.items():
            # 特徴量の競合回避ロジック
            if INCLUDE_OTHER_TARGETS_AS_FEATURES:
                # 音響特徴量 + 他の人気度指標
                final_features = base_features + other_pop_metrics
            else:
                # 音響特徴量のみ
                final_features = base_features
            
            # データフレームから抽出
            X_tr, X_va, X_te = train_df[final_features], val_df[final_features], test_df[final_features]
            
            # print(X_tr.head(10))
            # exit()

            # SVM実行
            test_ba = run_svm_logic(X_tr, y_train, X_va, y_val, X_te, y_test)
            
            all_results.append({
                'Target_Metric': target,
                'Experiment': exp_name,
                'Feature_Mode': mode_str,
                'Balanced_Accuracy': test_ba
            })
            print(f"  - {exp_name:25} | BA: {test_ba:.4f}")

    results_df = pd.DataFrame(all_results)
    
    pivot_df = results_df.pivot(index='Target_Metric', columns='Experiment', values='Balanced_Accuracy')
    print(f"\n--- Summary ({mode_str}) ---")
    print(pivot_df)

    # 結果保存    
    pivot_df.to_csv(f'./result_SVM/results_{mode_str}.csv', index=False)
    send_mail.prosess_mail("SVM処理完了\n", f"SVM.pyの処理が完了しました。結果ファイル: results_{mode_str}.csv")

if __name__ == "__main__":
    main()