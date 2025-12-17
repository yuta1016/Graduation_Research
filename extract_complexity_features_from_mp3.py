import numpy as np
import librosa
import os
from scipy.spatial.distance import jensenshannon
from scipy.special import softmax
from scipy.fftpack import fft
import for_features_excel

INPUT_DATA_FOLDER = "billboard_futures_info"
#OUTPUT_DATA_FOLDER = "billboard_futures_info_with_complexity"
OUTPUT_DATA_FOLDER = "test"
FILE_NAME = "2008_2025_complexity.xlsx"

class MusicComplexityFeatures:
    def __init__(self, file_path, sr=44100):
        self.file_path = file_path
        self.sr = sr
        # 2.97秒のウィンドウ、1秒ごとの移動 (論文記述に基づく)
        self.n_fft = int(2.97 * sr)
        self.hop_length = int(1.0 * sr)
        
        # 音声のロード
        self.y, _ = librosa.load(file_path, sr=sr)
        
        # 特徴量のキャッシュ
        self.chroma_seq = None
        self.timbre_seq = None
        self.rhythm_seq = None
        self.arousal_mean = None
        self.arousal_std = None

    def _normalize_distribution(self, vector):
        """
        ベクトルを確率分布（合計1, 正の値）に変換する。
        MFCCなど負の値を含むものはSoftmaxを使用。
        """
        # 値が全て0の場合は均等分布を返す
        if np.sum(np.abs(vector)) == 0:
            return np.ones(len(vector)) / len(vector)
        
        # Softmaxを使用して負の値を考慮しつつ正規化
        return softmax(vector)

    def extract_chroma(self):
        """
        Chroma特徴量の抽出 (12次元)
        論文[30]のNNLSに近い挙動として、CQTベースのクロマを使用し確率分布化する。
        先行研究（https://www.researchgate.net/profile/Matthias-Mauch-2/publication/220723830_Approximate_Note_Transcription_for_the_Improved_Identification_of_Difficult_Chords/links/0deec5193a24452331000000/Approximate-Note-Transcription-for-the-Improved-Identification-of-Difficult-Chords.pdf）、
        は対周波数スペクトルフレームを分解するために、2つの要素が必要↓ 
        we need two basic in-redients: a note dictionary E, describing the assumed pro-
        file of (idealised) notes, and an inference procedure to de-
        termine the note activation patterns that result in the closest
        match to the spectral frame

        ただ時間的に難しいのでlibrosaのcqt(対周波数を使った)を使用して近似する。
        """
        # 時間分解能を論文の "0.25 sec" に合わせるためのhop_length
        hop_length_chroma = int(0.25 * self.sr)
        
        # CQTクロマの計算
        chroma = librosa.feature.chroma_cqt(y=self.y, sr=self.sr, hop_length=hop_length_chroma)
        print(f"Chroma shape: {chroma.shape}")  # デバッグ用出力

        # 各フレームを確率分布として正規化 (列ごとに合計1にする)
        # axis=0は特徴量次元(12音階)
        self.chroma_seq = librosa.util.normalize(chroma, norm=1, axis=0).T # (Time, 12)
        return self.chroma_seq

    def extract_timbre(self):
        """
        Timbre (MFCC) 特徴量の抽出 (36次元)
        """
        timbre_list = []
        n_samples = len(self.y)
        
        # 1秒ごとにループ (hop_length)
        #A 2.97 second-long Hamming window moving one second at a time is applied to the audio signal. 
        for start in range(0, n_samples - self.n_fft, self.hop_length):
            end = start + self.n_fft
            segment = self.y[start:end]
            
            # MFCC計算
            # Thus,we employ the Mel-frequency cepstral coefficients(MFCCs) [32] that are popular for perceptual representation of spectral aspects of audio signals. 
            mfcc = librosa.feature.mfcc(y=segment, sr=self.sr, n_mfcc=36)
            
            # (36, Time) -> 時間方向に平均して (36,) のベクトルにする
            # Finally, the MFCCs from the 256 frames are averaged.
            mfcc_mean = np.mean(mfcc, axis=1)
            timbre_list.append(mfcc_mean)
            
        self.timbre_seq = np.array(timbre_list) # (Time, 36)
        
        # axis=0 (時間方向) に沿って、各次元(MFCC0, MFCC1...)ごとの最大・最小を使って正規化します。
        # 各次元の最小値と最大値を取得
        min_vals = np.min(self.timbre_seq, axis=0)
        max_vals = np.max(self.timbre_seq, axis=0)
        
        # ゼロ除算を防ぐための安全策
        range_vals = max_vals - min_vals
        range_vals[range_vals == 0] = 1.0 # 差が0なら割らない
        
        # 0〜1にスケーリング
        # https://aiacademy.jp/media/?p=1147
        self.timbre_seq = (self.timbre_seq - min_vals) / range_vals

        return self.timbre_seq
    

    def extract_rhythm(self):
        # 1. 設定値
        # 論文: 2.97秒ウィンドウ / 1秒ホップ
        # 44.1kHzの場合: 2.97s ≒ 131072サンプル (256 frames * 512 samples)
        # ※ self.sr が 22050Hz の場合はこれに合わせてスケールダウンしますが
        # 論文再現のため、内部的にパラメータを計算します。
        """
        Leeの論文からの引用：
        A 2.97 second-long Hamming window moving one second at a time is applied to the audio signal. The windowed audio segment is further divided into 256 frames (containing 512 samples for 44.1 kHzsampling).
        これらのための定義
        """
        samples_per_frame = 512  # 論文: "containing 512 samples"
        n_frames = 256           # 論文: "divided into 256 frames"
        window_size_samples = samples_per_frame * n_frames # 2.97秒分


        # Barkスケールフィルターバンクの作成 (librosaのMelで代用し、近似的にBarkとする)
        """
        https://www.researchgate.net/figure/The-BARK-scale-is-a-psycoacoustic-model-of-the-human-perception-of-loudness-in-relation_fig3_355664561
        ・上記の写真はBarkスケールの図。

        https://librosa.org/doc/main/generated/librosa.filters.mel.html
        ・melフィルターバンクの説明。Barkスケールに近似的に使える。

        rhythmの論文からの引用：
        First, a half wave rectified difference filter is applied on each Bark-band to emphasize percussive sounds.

        https://ccrma.stanford.edu/~jos/bbt/Bark_Frequency_Scale.html
        https://qiita.com/Oka_D/items/4622b5040deeb77c9bd4
        """
        n_mels = 24 # Bark帯域の数に近い値 (通常24バンド程度)
        mel_basis = librosa.filters.mel(sr=self.sr, n_fft=samples_per_frame, n_mels=n_mels)


        """
        Leeの論文からの引用：
        A 2.97 second-long Hamming window moving one second at a time is applied to the audio signal.
        ここに1秒ホップで2.97秒ウィンドウを適用する処理を実装と書かれているから
        """
        hop_length_seconds = 1.0
        hop_length_samples = int(hop_length_seconds * self.sr)



        # 4Hzを中心とした重み付けフィルター (Fluctuation Model)
        # n_frames(256)でフーリエ変換することで、配列ごとでhzが分かる
        frame_rate = self.sr / samples_per_frame
        mod_freqs = np.fft.rfftfreq(n_frames, d=1/frame_rate)
        #print("Modulation frequencies:", mod_freqs)  # デバッグ用出力
        
        # 4Hz (240bpm) にピークを持つガウス分布のような重みを作成
        # https://natorastats.com/normal-distribution/
        # 論文: "fluctuation model which has a peak at 4Hz"
        fluctuation_weight = np.exp(-0.5 * ((mod_freqs - 4) / 2) ** 2) # 中心4Hz, 広がり適当
        #print("Fluctuation weights:", fluctuation_weight)  # デバッグ用出力

        # ----------------------------------------------------
        # メインループ: 2.97秒ごとのウィンドウを1秒ずつずらす
        # ----------------------------------------------------
        rhythm_features = []
        n_total_samples = len(self.y)
        for start in range(0, n_total_samples - window_size_samples, hop_length_samples):
            # 2.97秒の切り出し
            chunk = self.y[start : start + window_size_samples]
            
            """
            2. スペクトログラムの計算 (Short-Time Fourier Transform)
            Leeの論文からの引用：
            A 2.97 second-long Hamming window moving one second at a time is applied to the audio signal. The windowed audio segment is further divided into 256 frames (containing 512 samples for 44.1 kHzsampling).
            256フレームに分割するために、n_fft=512, hop_length=512でSTFTを計算
            これにより、(Freq, Time=256) のスペクトログラムが得られる
            """
            D = librosa.stft(chunk, n_fft=samples_per_frame, hop_length=samples_per_frame, center=False)
            S = np.abs(D) # 振幅スペクトル (Freq, Time=256)

            # 3. Bark(Mel)帯域への変換
            # スペクトル情報を統合 (Bark, Time=256)
            bark_spec = np.dot(mel_basis, S)
            #print(f"Bark spectrogram shape: {bark_spec.shape}")  # デバッグ用出力

            # 論文にある "half wave rectified difference filter" の実装
            # 音の「変化（立ち上がり）」だけを取り出す処理
            #Bark spectrogram shape: (24, 256)
            # 時間方向(axis=1)の差分をとる
            diff_spec = np.diff(bark_spec, axis=1, prepend=bark_spec[:, :1])
            # 半波整流: マイナスの値（音が小さくなった時）を0にする
            rectified_spec = np.maximum(diff_spec, 0)
            #print(f"Rectified spectrogram shape: {rectified_spec.shape}")  # デバッグ用出力

            """
            One of the main differences between the FPs [4] and the PHs is that the FPs use a simple FFT instead of the computationally more expensive comb-filter to find periodicities in the Bark-bands.
            """
            # 4. 変調スペクトルの計算
            # ★ 修正: bark_spec ではなく rectified_spec を使う
            mod_spec = np.abs(np.fft.rfft(rectified_spec, axis=1))


            """Furthermore, while the PHs use a resonance model which has a maximum at about 120bpmtheFPsuseafluctuation model which has a peak at 4Hz (240bpm). The biggest difference, however, is that the FPs include information on the spectrum while the PHs disregard this information
             4Hz付近を強調"""
            # 5. Fluctuation Modelによる重み付け & 平均化
            weighted_mod_spec = mod_spec * fluctuation_weight
            
            # 全バンド、全変調周波数で合計または平均をとる
            # complexityから: "The mean of the 256 FPs is defined as the rhythm component"
            # Bark帯域(axis=0)方向だけ平均して、「変調周波数の分布」を残す。
            # これにより、rhythm_vec は (129,) のようなベクトルになる。
            rhythm_vec = np.mean(weighted_mod_spec, axis=0)
            
            rhythm_features.append(rhythm_vec)

        self.rhythm_seq = np.array(rhythm_features)
        
        #全体に対して標準化を行う (平均0, 分散1)
        # 値が小さすぎて0になってしまうのを防ぐ
        # 値の範囲を広げて、Softmaxがかかりやすくする
        if np.std(self.rhythm_seq) > 0:
            self.rhythm_seq = (self.rhythm_seq - np.mean(self.rhythm_seq)) / np.std(self.rhythm_seq)
        
        return self.rhythm_seq
    

    def extract_arousal(self):
        """
        Arousal (Loudness) の抽出
        Short-time magnitude (絶対値の和) の平均と標準偏差
        """
        # フレームごとの絶対値の和を計算
        # 信号をフレーム化
        # compの論文から: "2.97-second long Hamming window moving one second at a time"
        frames = librosa.util.frame(self.y, frame_length=self.n_fft, hop_length=self.hop_length)
        print(f"Frames shape for Arousal: {frames.shape}")  # デバッグ用出力
        
        # Hamming窓を適用
        #https://librosa.org/doc/latest/generated/librosa.util.frame.html
        window = np.hamming(self.n_fft)
        print(f"Window shape: {window.shape}")  # デバッグ用出力
        #https://note.nkmk.me/python-numpy-reshape-usage/#google_vignette
        window = window.reshape(-1, 1) # 放送用
        print(f"Reshaped Window shape: {window.shape}")  # デバッグ用出力
        
        windowed_frames = frames * window
        
        """compの論文からの引用：
        We calculate the short-time magnitude (i.e., sum of the absolute signal values) """
        magnitudes = np.sum(np.abs(windowed_frames), axis=0)
        

        """We then calculate the mean (ArousalMean) and standard deviation(ArousalStd) of the short-time magnitudes over time to measure the average and variation of loudness, respectively."""
        self.arousal_mean = np.mean(magnitudes)
        self.arousal_std = np.std(magnitudes)
        
        return {
            "ArousalMean": self.arousal_mean,
            "ArousalStd": self.arousal_std
        }


    def calculate_sc(self, component_seq, component_type):
        #Structural Change (SC) の計算

        n_segments = component_seq.shape[0]
        sc_values = {}

        # We set j = 3, 4, ..., 8 for chromaand j = 1, 2, ..., 6 for rhythm and timbre, which correspond to window lengths of 1 to 32 sec
        if component_type == 'chroma':
            js = range(3, 9)
        else:
            js = range(1, 7)

        for j in js:
            w_j = 2**(j - 1)
            
            jsd_list = []
            
            for i in range(w_j, n_segments - w_j):
                #Where JSD(x, y) is the Jenson-Shannon divergence between x and y and sa:b is the sum of the values of s for the ath to bth segments.
                vec_past = np.sum(component_seq[i - w_j : i], axis=0)
                vec_future = np.sum(component_seq[i : i + w_j], axis=0)
                
                # 正規化（Softmax）
                #JSDは「確率分布」しか受け付けない
                dist_past = self._normalize_distribution(vec_past)
                dist_future = self._normalize_distribution(vec_future)
                
                # JSD計算
                jsd = jensenshannon(dist_past, dist_future)
                
                # NaN / Inf 対策 (念のため)
                if not np.isfinite(jsd): jsd = 0.0
                
                jsd_list.append(jsd)
            
            """Finally, we take the mean value of SC over time to obtain a single value for each feature and each window size."""
            if len(jsd_list) > 0:
                sc_mean = np.mean(jsd_list)
            else:
                sc_mean = 0.0
                
            sc_values[f"{component_type.capitalize()}{j}"] = sc_mean
            
        return sc_values
    

    def get_all_complexity_features(self):
        # 1. 各コンポーネントの時系列データを抽出
        print("Extracting Chroma...")
        self.extract_chroma()
        print("Extracting Timbre...")
        self.extract_timbre()
        print("Extracting Rhythm...")
        self.extract_rhythm()
        
        # 2. Arousalの計算
        print("Calculating Arousal...")
        arousal_feats = self.extract_arousal()
        
        # 3. Structural Change (SC) の計算
        print("Calculating Structural Change (SC)...")
        features = {}
        
        # Chroma (j=3..8)
        sc_chroma = self.calculate_sc(self.chroma_seq, 'chroma')
        features.update(sc_chroma)
        
        # Rhythm (j=1..6)
        sc_rhythm = self.calculate_sc(self.rhythm_seq, 'rhythm')
        features.update(sc_rhythm)
        
        # Timbre (j=1..6)
        sc_timbre = self.calculate_sc(self.timbre_seq, 'timbre')
        features.update(sc_timbre)
        
        # Arousalを追加
        features.update(arousal_feats)
        
        return features


# --- 実行例 ---
if __name__ == "__main__":
    # 音楽ファイルのパスを指定してください
    #audio_file = "./downloaded_mp3/星野源_ドラえもん.mp3"
    FeaturesExcelSaver = for_features_excel.FeaturesExcelSaver()

    csv_files = FeaturesExcelSaver.find_input_csv_files(INPUT_DATA_FOLDER)
    mp3_paths_dict, csv_data = FeaturesExcelSaver.retrive_mp3_path(csv_files, 4, 3)  # MP3パスが6列目、URLが3列目

    complexity_features_list = {}
    for url, audio_file in mp3_paths_dict.items():
        print(f"\nProcessing URL: {url}")

        extractor = MusicComplexityFeatures(audio_file)
        features = extractor.get_all_complexity_features()
        complexity_features_list[url] = features
        
        print("\n--- Calculated Features ---")
        for k, v in features.items():
            print(f"{k}: {v}")


    FeaturesExcelSaver.save_features_to_excel(complexity_features_list, csv_data, OUTPUT_DATA_FOLDER, FILE_NAME)

    
    # ファイルが存在するか確認用ダミーチェック (実際には外してください)
    # if os.path.exists(audio_file):
    #     extractor = MusicComplexityFeatures(audio_file)
    #     features = extractor.get_all_complexity_features()
        
    #     print("\n--- Calculated Features ---")
    #     for k, v in features.items():
    #         print(f"{k}: {v}")
    # else:
    #     print("音楽ファイルを指定してください。")