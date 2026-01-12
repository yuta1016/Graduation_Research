import pandas as pd
import numpy as np
from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import balanced_accuracy_score
import send_mail
import csv
import os
import random
from bayes_opt import BayesianOptimization

# ==========================================
# 1. 設定 (CONFIGURATION)
# ==========================================

# --- ファイルパス設定 ---
# PATH_TRAIN = './billboard_futures_info_by_age/train_2008_2017.csv'
# PATH_VAL = './billboard_futures_info_by_age/val_2018_2021.csv'
# PATH_TEST = './billboard_futures_info_by_age/test_2022_2025.csv'

# YEARS_TRAIN = ["2015", "2016", "2017", "2018"]
# YEARS_VAL   = ["2019"]
# YEARS_TEST  = ["2020"]
START_YEAR = 2008
END_YEAR = 2025

TRAING_REN = 4
VAL_LEN = 1
TEST_LEN = 1

OUTPUT_PATH = 'result_SVM'
#OUTPUT_PATH_PER = 'median_val/median_val_traing_70_per'
OUTPUT_PATH_PER = "bayesian_optimization/bays_rbf"

PATH_COMPLEXITY = './features_complexity/2008_2025_complexity.csv'
PATH_MFCC = './features_mfcc/2008_2025_mfcc.csv'

# --- ターゲット設定 (8つの人気度指標) ---
TARGET_COLS = ["Max", "Mean", "Std", "Length", "Sum", "Skewness", "Kurtosis", "Debut_Score",]

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
    def __init__(self, comp_path, mfcc_path):
        self.comp_path = comp_path
        self.mfcc_path = mfcc_path
        self.dfs = None
        self.shapes = []
        self.traing_per = None

    def data_shapes(self):
        train_shape = self.shapes[0][0]
        val_shape = self.shapes[1][0]
        test_shape = self.shapes[2][0]
        traing_per = self.traing_per
        return train_shape, val_shape, test_shape, traing_per


    def load_csv(self, years_train, years_val, years_test):
        def load_logic(years):
            dfs = []
            for year in years:
                path = f'./billboard_futures/billboard_features_{year}.csv'
                df = pd.read_csv(path)
                dfs.append(df)
            return pd.concat(dfs, ignore_index=True)
        
        train_df = load_logic(years_train)
        val_df = load_logic(years_val)
        test_df = load_logic(years_test)

        print("Loaded Data Shapes:")
        self.shapes = [train_df.shape, val_df.shape, test_df.shape]
        print(f"Train: {train_df.shape}\nVal: {val_df.shape}\nTest: {test_df.shape}")

        self.traing_per = len(train_df) / (len(train_df) + len(val_df) + len(test_df)) * 100
        print(f"traing_data(%) = {self.traing_per:.2f}%")

        self.dfs = [train_df, val_df, test_df]


    def load_and_merge(self):
        df_comp = pd.read_csv(self.comp_path)
        df_mfcc = pd.read_csv(self.mfcc_path)

        def merge_logic(base_df):
            tmp = pd.merge(base_df, df_comp, on='URL', how='inner', suffixes=('', '_comp'))
            tmp = pd.merge(tmp, df_mfcc, on='URL', how='inner', suffixes=('', '_mfcc'))
            return tmp

        return [merge_logic(p) for p in self.dfs]

def run_svm_logic(X_train, y_train, X_val, y_val, X_test, y_test):
    # 数値データのみを抽出 (念のため)
    X_train = X_train.select_dtypes(include=[np.number])
    X_val = X_val.select_dtypes(include=[np.number])
    X_test = X_test.select_dtypes(include=[np.number])

    scaler = StandardScaler()
    X_train_s = scaler.fit_transform(X_train)
    X_val_s = scaler.transform(X_val)
    X_test_s = scaler.transform(X_test)

    # C_range = [0.1, 1, 10, 100]
    # gamma_range = [0.001, 0.01, 0.1, 1, 'scale']

    best_ba_val = -1
    best_model = None

    # --- Grid Search (Commented Out) ---
    # for C in C_range:
    #     for gamma in gamma_range:
    #         clf = SVC(kernel='rbf', C=C, gamma=gamma, class_weight='balanced')
    #         clf.fit(X_train_s, y_train)
    #         val_preds = clf.predict(X_val_s)
    #         ba_val = balanced_accuracy_score(y_val, val_preds)
            
    #         if ba_val > best_ba_val:
    #             best_ba_val = ba_val
    #             best_model = clf

    # --- Bayesian Optimization ---
   # --- Bayesian Optimization (過学習抑制版) ---
    def svm_eval(log_C, log_gamma):
        C = 10 ** log_C
        gamma = 10 ** log_gamma
        
        clf = SVC(kernel='rbf', C=C, gamma=gamma, class_weight='balanced', random_state=42)
        clf.fit(X_train_s, y_train)
        
        # TrainとVal両方の予測を行う
        train_preds = clf.predict(X_train_s)
        val_preds = clf.predict(X_val_s)
        
        score_tr = balanced_accuracy_score(y_train, train_preds)
        score_val = balanced_accuracy_score(y_val, val_preds)
        
        # 【重要】過学習ペナルティ項
        # 学習スコアと検証スコアの差（gap）を計算
        gap = score_tr - score_val
        
        # 差が大きすぎる場合（過学習）、スコアを減点する
        # 係数 0.1 ～ 0.5 程度で調整（ここでは0.2としてみます）
        # もし gap が負（Valの方が良い）ならペナルティは0にする
        penalty = 0.2 * max(0, gap)
        
        return score_val - penalty



    # 探索範囲 (対数スケール: 10^-3 ~ 10^3 程度)
    pbounds = {'log_C': (-4, 1), 'log_gamma': (-4, -1)}

    optimizer = BayesianOptimization(f=svm_eval, pbounds=pbounds, random_state=42, verbose=0)
    optimizer.maximize(init_points=5, n_iter=50)

    best_params = optimizer.max['params']
    best_C = 10 ** best_params['log_C']
    best_gamma = 10 ** best_params['log_gamma']
    print(f"\n✅ Optimization Finished")
    print(f"  - Best Params: C={best_C:.4f}, gamma={best_gamma:.4f}")

    # 6. 最適パラメータで再学習 (Trainデータのみ使用)
    best_model = SVC(kernel='rbf', C=best_C, gamma=best_gamma, class_weight='balanced', random_state=42)

    #-------------------------------------------------------
    #print("こっから描写ーーーーーーーーーーーーーーーーーーー")
    
    # ★ここから学習曲線の描画ロジックを追加・修正★
    from sklearn.model_selection import learning_curve
    import matplotlib.pyplot as plt

    # 学習曲線の計算
    # cv=5 は学習データ(Train)をさらに5分割して検証することを意味します
    # train_sizes, train_scores, valid_scores = learning_curve(
    #     estimator=best_model,
    #     X=X_train_s, y=y_train,
    #     train_sizes=np.linspace(0.1, 1.0, 10),
    #     cv=5,
    #     scoring='balanced_accuracy',
    #     n_jobs=-1
    # )

    # # 平均と標準偏差の計算
    # train_mean = np.mean(train_scores, axis=1)
    # train_std  = np.std(train_scores, axis=1)
    # valid_mean = np.mean(valid_scores, axis=1)
    # valid_std  = np.std(valid_scores, axis=1)

    # # プロットの作成
    # plt.figure(figsize=(8, 6))
    
    # # Training Score (青)
    # plt.plot(train_sizes, train_mean, color='blue', marker='o', markersize=5, label='Training score')
    # plt.fill_between(train_sizes, train_mean + train_std, train_mean - train_std, alpha=0.15, color='blue')
    
    # # Validation Score (緑)
    # plt.plot(train_sizes, valid_mean, color='green', linestyle='--', marker='o', markersize=5, label='Cross-validation score')
    # plt.fill_between(train_sizes, valid_mean + valid_std, valid_mean - valid_std, alpha=0.15, color='green')

    # # グラフの装飾
    # plt.title('Learning Curve (SVM)')
    # plt.xlabel('Training examples')
    # plt.ylabel('Balanced Accuracy')
    # plt.legend(loc='lower right')
    # plt.grid()
    
    # # グラフの表示 (実行環境によっては plt.savefig("learning_curve.png") 推奨)
    # plt.show()


    # print("以下からバリデーション曲線ーーーーーーーーーーーーーーーーーーー")
    # from sklearn.model_selection import validation_curve

    # # 変数の定義
    # model = best_model
    # X = X_train_s
    # y = y_train
    # cv = 5
    # scoring = 'balanced_accuracy'

    # cv_params = {'gamma': [0.0001, 0.001, 0.01, 0.03, 0.1, 0.3, 1, 3, 10, 100, 1000],
    #         'C': [0.001, 0.01, 0.1, 0.3, 1, 3, 10, 100, 1000]}
    
    # for i, (k, v) in enumerate(cv_params.items()):
    #     plt.figure(figsize=(8, 6))
    #     train_scores, valid_scores = validation_curve(estimator=model,
    #                                                 X=X, y=y,
    #                                                 param_name=k,
    #                                                 param_range=v,
    #                                                 cv=cv, scoring=scoring,
    #                                                 n_jobs=-1)

    #     # 学習データに対するスコアの平均±標準偏差を算出
    #     train_mean = np.mean(train_scores, axis=1)
    #     train_std  = np.std(train_scores, axis=1)
    #     train_center = train_mean
    #     train_high = train_mean + train_std
    #     train_low = train_mean - train_std
    #     # テストデータに対するスコアの平均±標準偏差を算出
    #     valid_mean = np.mean(valid_scores, axis=1)
    #     valid_std  = np.std(valid_scores, axis=1)
    #     valid_center = valid_mean
    #     valid_high = valid_mean + valid_std
    #     valid_low = valid_mean - valid_std
    #     # training_scoresをプロット
    #     plt.plot(v, train_center, color='blue', marker='o', markersize=5, label='training score')
    #     plt.fill_between(v, train_high, train_low, alpha=0.15, color='blue')
    #     # validation_scoresをプロット
    #     plt.plot(v, valid_center, color='green', linestyle='--', marker='o', markersize=5, label='validation score')
    #     plt.fill_between(v, valid_high, valid_low, alpha=0.15, color='green')
    #     # スケールを'log'に（線形なパラメータは'linear'にするので注意）
    #     plt.xscale('log')
    #     # 軸ラベルおよび凡例の指定
    #     plt.xlabel(k)  # パラメータ名を横軸ラベルに
    #     plt.ylabel(scoring)  # スコア名を縦軸ラベルに
    #     plt.legend(loc='lower right')  # 凡例
    #     plt.title(f'Validation Curve ({k})')
    #     # グラフを描画
    #     plt.show()

    # print("ここまで描写ーーーーーーーーーーーーーーーーーーー")

    best_model.fit(X_train_s, y_train)
    test_preds = best_model.predict(X_test_s)
    #print("予測したよ")

    return balanced_accuracy_score(y_test, test_preds)


# ==========================================
# 3. メイン処理
# ==========================================

def run_experiment(years_train, years_val, years_test):
    loader = DataLoader(PATH_COMPLEXITY, PATH_MFCC)

    loader.load_csv(years_train, years_val, years_test)
    train_df, val_df, test_df = loader.load_and_merge()

    #先行研究と比べるために
    # if len(train_df) >= 864:
    #     train_df = train_df.sample(n=864, random_state=42)
    # if len(val_df) >= 200:
    #     val_df = val_df.sample(n=200, random_state=42)
    # if len(test_df) >= 200:
    #     test_df = test_df.sample(n=200, random_state=42)


    # print(f"\nData after merging features:")
    # print(f"train:{train_df.columns}::: shape={train_df.shape}")
    # print(f"val:{val_df.head(3)}::: shape={val_df.shape}")
    # print(f"test:{test_df.head(3)}::: shape={test_df.shape[0]}")
    #exit()
    
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
            Name: count, dtype: int64
        """
        median_val = train_df[target].median()
        y_train = (train_df[target] > median_val).astype(int)
        y_val = (val_df[target] > median_val).astype(int)
        y_test = (test_df[target] > median_val).astype(int)

        # print(f"y_train:{y_train.value_counts()}")
        # print(f"y_val:{y_val.value_counts()}")
        # print(f"y_test:{y_test.value_counts()}")
        

        # print(y_train.head(10))
        # exit()

        # 特徴量リストの作成
        # 現在のターゲット以外の人気度指標を特定
        other_pop_metrics = [t for t in TARGET_COLS if t != target]
        
        # 実験設定の定義
        experiments = {
            #"Foe example" : SPOTIFY_POPULARITY,
            "Exp1: Group_Complexity": FEAT_COMPLEXITY,
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

    #各実験の結果平均を追加
    pivot_df.loc['Average'] = pivot_df.mean()

    # 結果保存
    save_dir = os.path.join(OUTPUT_PATH, OUTPUT_PATH_PER)
    os.makedirs(save_dir, exist_ok=True)

    save_path = os.path.join(save_dir, f'results_{years_train[0]}_{years_test[-1]}.csv')
    pivot_df.to_csv(save_path, index=True)

    with open(save_path, mode='a', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        
        # 2行空ける
        f.write('\n\n\n')
        
        # 年情報を書き込む
        # リスト同士を足し算 [+] することで横に並べます
        writer.writerow(['training'] + years_train)
        writer.writerow(['val'] + years_val)
        writer.writerow(['test'] + years_test)

        f.write('\n\n\n')

        train_shape, val_shape, test_shape, traing_per = loader.data_shapes()

        writer.writerow(['Data Shapes'])
        writer.writerow(['traing_data(%)', traing_per])
        writer.writerow(['Train', train_shape])
        writer.writerow(['Val', val_shape])
        writer.writerow(['Test', test_shape])


def main():
    # 年代設定 (2008年から2025年まで)
    start_year = START_YEAR
    end_year = END_YEAR
    all_years = [str(y) for y in range(start_year, end_year + 1)]

    # print(f"All Years: {all_years}")
    # print(f"Total Years: {len(all_years)}")
    # print("---------------------------------------")
    # random_years = random.sample(all_years, TRAING_REN)
    # print(f"Random Years: {random_years}")
    # print(f"Total Random Years: {len(random_years)}")
    # exit()

    # ウィンドウサイズ設定
    train_len = TRAING_REN
    val_len = VAL_LEN
    test_len = TEST_LEN
    total_len = train_len + val_len + test_len

    #print(f"\n=== Processing Window: Train={YEARS_TRAIN}, Val={YEARS_VAL}, Test={YEARS_TEST} ===")
    #run_experiment(YEARS_TRAIN, YEARS_VAL, YEARS_TEST)

    # スライディングウィンドウ実行
    for i in range(len(all_years) - total_len + 1):
        years_train = all_years[i : i + train_len]
        years_val = all_years[i + train_len : i + train_len + val_len]
        years_test = all_years[i + train_len + val_len : i + total_len]

        # years_test = all_years[i + train_len + val_len : i + total_len]
        # years_train = random.sample([y for y in all_years if y not in years_test], train_len)
        # years_val = random.sample([y for y in all_years if y not in years_train and y not in years_test], val_len)

        print(f"Selected Years - Train: {years_train}, Val: {years_val}, Test: {years_test}")
        
        run_experiment(years_train, years_val, years_test)

    send_mail.prosess_mail(f"SVM処理完了\nSVM.pyの処理が完了しました。期間: {years_train[0]}-{years_test[-1]}")

if __name__ == "__main__":
    main()