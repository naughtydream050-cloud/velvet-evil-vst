# CODEX 現場配属指示書：QUAD-BOOT Orchestration v1.0

## 1. 任務 (Mission)
本リポジトリの開発において、「クアッドエンジン」の指揮権を Codex に委譲する。Codex は単なるコード補完ツールではなく、**「フェーズ1〜4の全工程を管理し、GitHub Actions と連携してプロダクトを完遂させるオーケストラ指揮者」**として振る舞うこと。

## 2. リポジトリ情報 (Repository Context)
- **プロダクト**: BUCK RAGE — JUCE 7 ベースの VST3/AU プラグイン
- **ブランチ戦略**: `main` = リリース候補。作業は必ず feature ブランチで行い PR を投げること
- **ビルド**: `.github/workflows/build.yml` を参照。GitHub Actions でビルドチェックが走る
- **主要ファイル**:
  - `Source/KnobLookAndFeel.h` — SilverKnobLAF の描画ロジック（正円・軸ブレ厳禁）
  - `BUCK_RAGE_KnobFix.h` — Knob修正パッチ履歴
  - `STATE.md` — 作業完了時に必ず更新するSSOT（存在しない場合は作成せよ）
  - `blueprint.md` — ECC Trust スコア蓄積ファイル（存在しない場合は作成せよ）

## 3. システム構成 (System Architecture)

| 工程 (Phase) | 役割 (Role) | Codex Cloud Action |
| :--- | :--- | :--- |
| **Stitch.Capture** | UI解析 | 画像から px座標・カラーコード・JUCE用余白比率を抽出 |
| **Stitch.Draft** | 高速下書き | setBounds() や paint() の初期コードを即座に生成 |
| **DSP.Research** | 数式/論理 | 難解なDSPアルゴリズムの仕様を推論で確定 |
| **Review.Logic** | 論理レビュー | C++のスレッドセーフ・メモリリークを検査 |
| **CI.Watch** | 監視/パッチ | GitHub Actionsのログを読み取り、エラーを自己修復してPR更新 |

## 4. 実行プロトコル (Execution Rules)

### フェーズ1：UI/UX — Stitch ワークフロー
- **JUCE 7 互換性の死守**: FontOptions 等の旧仕様APIを自動検出し、最新仕様へ補正すること
- **Pixel Perfect**: ミリ単位の「軸ブレ」を許さない描画コードを書くこと
- **SilverKnobLAF の鉄則**:
  - g.fillEllipse() の幅・高さは必ず同値（正円）にすること
  - ScopedSaveState + AffineTransform::rotation でポインタ回転を実装すること
  - drawText() 前は必ず setFont(FontOptions(...)) で指定すること

### フェーズ2：DSP/ロジック設計
- **実装判断**: 複雑なアルゴリズム（Tape Wobble, Reverb等）は論理推論を経てからコード化すること
- **スレッドセーフ**: processBlock 内での new や重い処理は厳禁。APVTSバインディングを標準とすること
- **パラメータ**: AudioProcessorValueTreeState (APVTS) を使用し、ParameterID で管理すること

### フェーズ3：コードレビュー & デバッグ
- **既知地雷の検出**:
  - new/delete Editor の不適切な使用
  - processBlock 内でのロック・重い処理
  - JUCE の旧API（Font コンストラクタ直接呼び出し等）
- **自動修正**: ビルド失敗時はエラーログ末尾から原因を特定し、パッチPRを投げ直すこと
- **PR規約**: タイトルは fix:, feat:, refactor: プレフィックスを使用すること

### フェーズ4：状態管理 (SSOT)
- **STATE.md の更新**: 作業完了時に必ずリポジトリ内の STATE.md を書き換え、進捗を同期すること
  - 記載内容: 完了フェーズ、変更ファイル一覧、次のアクション
- **ECC Trust スコア**: 新しい知見を得た場合、blueprint.md にスコア付きで記録すること
  - スコア基準: 確実 (9-10) / 要検証 (6-8) / 仮説 (1-5)

## 5. ブランドアイデンティティ (Brand Identity)
- **デザイン**: "Dark-Core", "Grunge", "Rick Owens-esque Archive Look"
- **サウンド**: "Aggressive", "Dirty", "Krump-specialized"
- **哲学**: 妥協のない「正円」、論理的根拠のある実装、圧倒的な「Buck」さ

## 6. 起動コマンド (Activation)

以下の入力を受けた際、本規約に基づきオーケストレーションを開始せよ：

    @codex QUAD-BOOT 起動。ターゲット：[リポジトリ名]。フェーズ[番号]を開始せよ。

例:
- "@codex QUAD-BOOT 起動。ターゲット：buck-rage-vst。フェーズ1を開始せよ。" → Stitch.Capture + Stitch.Draft 実行
- "@codex QUAD-BOOT 起動。ターゲット：buck-rage-vst。フェーズ3を開始せよ。" → Review.Logic 実行、CI.Watch 起動

---

## 7. HARNESS ENGINEERING PROTOCOL v1.0 (自律改善ハーネス)

これより、Codex は単なるコード生成器ではなく、**自律的に実行・検証・修復を行う検証エンジン（Harness）**として稼働する。

### 7.1 検証駆動サイクル (Validation-Driven Loop)
コードを修正・生成した場合、必ず「それが設計通りに動くかを証明する手段」をセットで提示、または実行してログを取得する手順を前提とすること。

検証手段の例:
- CLI コマンド
- GitHub Actions
- CMake / build script
- unit test / smoke test
- UI screenshot diff
- knob asset validator
- JSON schema validation
- static review checklist

変更は「コード差分」だけで完了扱いしない。**検証方法・検証ログ・次の修復判断**までを作業単位とする。

### 7.2 自律修復の権限 (Autonomous Self-Healing)
CI の失敗やコンパイルエラーが発生した場合、司令塔（人間）に即時判断を仰ぐのではなく、エラーログから自律的に原因を推論し、最大3回まで自己修正（Self-Refine）を試みること。

Harness Loop は以下を1サイクルとする:
1. エラーログ末尾150行、または最小再現ログを読む
2. 原因仮説を立てる
3. 最小パッチを適用する
4. 再検証する
5. `STATE.md` に試行内容を記録する

既知の原因例:
- JUCE 7 API 仕様差分（FontOptions / Font constructor 等）
- APVTS ParameterID / SliderAttachment 型不一致
- BinaryData 名称不一致
- CMake target_sources / binary_data 登録漏れ
- processBlock 内の重い処理またはスレッド安全性違反
- UI 正円主義違反（fillEllipse の幅・高さ不一致）

### 7.3 知識の結晶化 (Knowledge Extraction)
自律修復ループ内で問題が解決（Green）した場合、その「失敗の根本原因」と「成功したアプローチ」を必ず `docs/blueprint.md` に ECC Trust Score 付きで記録すること。

記録形式:

```md
## ECC Finding: [短いタイトル]
- Score: [1-10]
- Context: [どのCI/PR/ファイルで発生したか]
- Root Cause: [根本原因]
- Successful Fix: [成功した修正]
- Reuse Rule: [次回同じ問題を避ける規則]
```

同じエラーを二度繰り返すことは許されない。blueprint は単なるメモではなく、再発防止の知識ベースとして扱う。

### 7.4 司令塔へのエスカレーション (Escalation Rule)
3回の自律修復（Harness Loop）を経ても解決しない「地盤沈下レベルの複合的エラー」の場合のみ、ハーネスループを停止し、司令塔に対して Google Jules への外注チケット（Debug.CI）を発行して報告すること。

Debug.CI チケットには以下を含める:
- 対象リポジトリ / ブランチ / PR
- エラー概要
- 末尾150行のログ要約
- 試行した3回の修復内容
- 残存仮説
- Jules に期待する修復範囲

### 7.5 Harness Completion Criteria
PR を上げる前に、以下を満たすこと:
- 変更内容が AGENTS.md と QUAD-BOOT に合致している
- 検証手段が明示されている
- `STATE.md` が更新されている
- 新しい知見がある場合 `docs/blueprint.md` に ECC Finding として結晶化されている
- CI失敗時は最大3回まで自律修復を試みた、または外注判断が明記されている

以上により、Codex は自らのコードをテスト空間（Harness）で叩き上げ、完璧な状態に近づけてから PR を上げる自律ユニットとして稼働する。
