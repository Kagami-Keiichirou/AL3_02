#include "GameScene.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#include <cassert>

// コントストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete spriteBG_;       // BG
	delete modelStage_;     // ステージ
	delete modelPlayer_;    // プレイヤー
	delete modelBeam_;      // ビーム
	delete modelEnemy_;     // 敵
	delete spriteTitle_;    // タイトル
	delete spriteEnter_;    // エンター
	delete spriteGameOver_; // ゲームオーバー
}

// 初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	// タイトル(2Dスプライト)
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {390, 500});

	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	for (int n = 0; n < 5; n++) {
		textureHandleNumber_ = TextureManager::Load("score.png");
		spriteNumber_[n] = Sprite::Create(textureHandleNumber_, {0, 0});
	}

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	for (int s = 0; s < 20; s++) {
		worldTransformStage_[s].Initialize();

		// ステージの位置を変更
		worldTransformStage_[s].translation_ = {0, -1.5f, 2.0f * s - 5};
		worldTransformStage_[s].scale_ = {4.5f, 1, 1};

		// 変数行列を更新
		worldTransformStage_[s].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[s].scale_,
			worldTransformStage_[s].rotation_,
		    worldTransformStage_[s].translation_);

		// 変更行列を定数バッファに転送
		worldTransformStage_[s].TransferMatrix();
	}

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();
	}

	// 敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	// 敵の数ループする
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}

	srand((unsigned int)time(NULL));

	// デバッグテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	// スコア数値(2Dスプライト)
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleScore_, {150, 0});
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 20, 0});
	}

	//ライフ(2Dスプライト)
	for (int i = 0; i < 3; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 10});
		spriteLife_[i]->SetSize({40, 40});
	}

	// サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleGameClearBGM_ = audio_->LoadWave("Audio/fanfare.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");

	// タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
}

// 更新
void GameScene::Update() {
	// 各シーンの更新処理を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate(); // ゲームプレイ更新
		// ゲームタイマー
		gameTimer_++;
		break;
	case 1:
		TitleUpdate(); // タイトル更新
		gameTimer_++;
		break;
	case 2:
		GameOverUpdate();
		gameTimer_++;
		break;
	case 3:
		GameClearUpdate();
		gameTimer_++;
		break;
	}
}

void GameScene::GamePlayUpdate() {
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
	StageUpdate();
	Collision();

	// プレイヤーライフが0以下になったとき
	if (playerLife_ <= 0) {
		sceneMode_ = 2;

		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleGameOverBGM_, true); // ゲームオーバーBGMを再生
	}

	if (gameScore_ > 100) {
		gameScore_ = 100;
		sceneMode_ = 3;

		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleGameClearBGM_, true); // ゲームオーバーBGMを再生
	}
}

//----------------------------------------------
// タイトル
//----------------------------------------------

// タイトル更新
void GameScene::TitleUpdate() {
	// エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 0;
		GamePlayStart();

		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleGamePlayBGM_, true); // ゲームプレイBGMを再生
	}
}

// タイトル表示
void GameScene::TitleDraw2DNear() {
	// タイトル表示
	spriteTitle_->Draw();

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::GameOverUpdate() {
	// エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 1;
		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_);	// 現在のBGMを停止
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true); // タイトルBGMを再生
	}
}

void GameScene::GameOverDraw2DNear() {
	// タイトル表示
	spriteGameOver_->Draw();

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::GameClearUpdate() {
	// エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 1;
		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_);		// 現在のBGMを停止
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true); // タイトルBGMを再生
	}
}

void GameScene::GameClearDraw2DNear() {

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

//----------------------------------------------
// ステージ
//----------------------------------------------

// ステージ更新
void GameScene::StageUpdate() {
	// 各ステージでループ
	for (int s = 0; s < 20; s++) {
		// 手前に移動
		worldTransformStage_[s].translation_.z -= 0.1f;
		// 端まで来たら奥へ戻る
		if (worldTransformStage_[s].translation_.z < -5) {
			worldTransformStage_[s].translation_.z += 40;
		}
		// 変換行列を更新
		worldTransformStage_[s].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[s].scale_,
			worldTransformStage_[s].rotation_,
		    worldTransformStage_[s].translation_);
		// 変数行列を定数バッファに転送
		worldTransformStage_[s].TransferMatrix();
	}
}

//----------------------------------------------
// プレイヤー
//----------------------------------------------

// プレイヤー更新
void GameScene::PlayerUpdate() {
	// 移動

	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}

	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

	// 右への移動制限
	if (worldTransformPlayer_.translation_.x > 4) {
		worldTransformPlayer_.translation_.x = 4;
	}

	// 左への移動制限
	if (worldTransformPlayer_.translation_.x < -4) {
		worldTransformPlayer_.translation_.x = -4;
	}

	// 変数行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	if (playerTimer_ > 0) {
		playerTimer_--;
	}

	// 変数行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
}

//----------------------------------------------
// ビーム
//----------------------------------------------

// ビーム更新
void GameScene::BeamUpdate() {
	// ビーム移動
	BeamMove();
	// ビーム発生
	BeamBorn();
	// 変数行列を更新
	for (int b = 0; b < 10; b++) {
		worldTransformBeam_[b].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[b].scale_, worldTransformBeam_[b].rotation_,
		    worldTransformBeam_[b].translation_);

		// 変数行列を定数バッファに転送
		worldTransformBeam_[b].TransferMatrix();
	}
}

// ビーム移動
void GameScene::BeamMove() {
	for (int b = 0; b < 10; b++) {
		if (beamFlag_[b] == 1) {
			worldTransformBeam_[b].translation_.z += 0.3f;
		}
		worldTransformBeam_[b].rotation_.x += 0.1f;
	}
}

// ビーム発生 (発射)
void GameScene::BeamBorn() {
	for (int b = 0; b < 10; b++) {
		if (beamTimer_ == 0) {
			if (input_->PushKey(DIK_SPACE)) {
				if (beamFlag_[b] == 0) {
					worldTransformBeam_[b].translation_.x = worldTransformPlayer_.translation_.x;
					worldTransformBeam_[b].translation_.y = worldTransformPlayer_.translation_.y;
					worldTransformBeam_[b].translation_.z = worldTransformPlayer_.translation_.z;
					beamFlag_[b] = 1;
					beamTimer_ = 1;
					break;
				}
			}
			if (worldTransformBeam_[b].translation_.z > 40) {
				beamFlag_[b] = 0;
			}
		} else {
			// 発射タイマーが1以上
			// 10を超えると再び発射が可能
			beamTimer_++;
			if (beamTimer_ > 100) {
				beamTimer_ = 0;
			}
		}
	}
}

//----------------------------------------------
// 敵
//----------------------------------------------

// 敵更新
void GameScene::EnemyUpdate() {
	// 敵移動
	EnemyMove();
	// 敵発生
	EnemyBorn();
	// 敵ジャンプ
	EnemyJump();
	// 変数行列を更新
	for (int e = 0; e < 10; e++) {
		worldTransformEnemy_[e].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[e].scale_, worldTransformEnemy_[e].rotation_,
		    worldTransformEnemy_[e].translation_);

		// 変数行列を定数バッファに転送
		worldTransformEnemy_[e].TransferMatrix();
	}
}

// 敵移動
void GameScene::EnemyMove() {
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[e] == 1) {
			worldTransformEnemy_[e].translation_.z -= 0.1f;
			worldTransformEnemy_[e].rotation_.x -= 0.1f;
			worldTransformEnemy_[e].translation_.x += enemySpeed_[e];
			worldTransformEnemy_[e].translation_.z -= gameTimer_ / 1000.0f;
			if (worldTransformEnemy_[e].translation_.z < -5) {
				enemyFlag_[e] = 0;
			}

			if (worldTransformEnemy_[e].translation_.x >= 4) {
				enemySpeed_[e] = -0.1f;
			}

			if (worldTransformEnemy_[e].translation_.x <= -4) {
				enemySpeed_[e] = 0.1f;
			}
		}
	}
}

// 敵発生
void GameScene::EnemyBorn() {
	// 乱数で発生
	if (rand() % 10 == 0) {
		for (int e = 0; e < 10; e++) {
			if (enemyFlag_[e] == 0) {
				enemyFlag_[e] = 1;
				worldTransformEnemy_[e].translation_.z = 40;
				worldTransformEnemy_[e].translation_.y = 0;

				// 乱数でX座標の指定
				int x = rand() % 80;          // 80は4の10倍の2倍
				float x2 = (float)x / 10 - 4; // 10で割り、4を引く
				worldTransformEnemy_[e].translation_.x = x2;

				// 敵スピード
				if (rand() % 2 == 0) {
					enemySpeed_[e] = 0.1f;
				} else {
					enemySpeed_[e] = -0.1f;
				}

				// ループ処理終了
				break;
			}
		}
	}
}

void GameScene::EnemyJump() {
	// 敵でループ
	for (int j = 0; j < 10; j++) {
		// 消滅演出ならば
		if (enemyFlag_[j] == 2) {
			// 移動(Y座標に速度を加える)
			worldTransformEnemy_[j].translation_.y += enemyJumpSpeed_[j];
			// 速度を減らす
			enemyJumpSpeed_[j] -= 0.1f;
			// 斜め移動
			worldTransformEnemy_[j].translation_.x += enemySpeed_[j] * 4;
			// 下へ落ちると消滅
			if (worldTransformEnemy_[j].translation_.y < -3) {
				enemyFlag_[j] = 0; // 存在しない
			}
		}
	}
}

//----------------------------------------------
// 衝突判定
//----------------------------------------------

// 衝突判定
void GameScene::Collision() {
	// 衝突判定(プレイヤーと敵)
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
}

// 衝突判定(プレイヤーと敵)
void GameScene::CollisionPlayerEnemy() {
	// 敵が存在すれば
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[e] == 1) {
			// 差を求める
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[e].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[e].translation_.z);
			// 衝突したら
			if (dx < 1 && dz < 1) {
				// 存在しない
				enemyFlag_[e] = 0;

				// ライフ減算
				playerLife_ -= 1;

				playerTimer_ = 60;

				// プレイヤーヒットSE
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
			}
		}
	}
}

// 衝突判定(ビームと敵)
void GameScene::CollisionBeamEnemy() {
	// 敵が存在すれば
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[e] == 1) {
			for (int b = 0; b < 10; b++) {
				if (beamFlag_[b] == 1) {
					// 差を求める
					float ex =
					    abs(worldTransformBeam_[b].translation_.x -
					        worldTransformEnemy_[e].translation_.x);
					float ez =
					    abs(worldTransformBeam_[b].translation_.z -
					        worldTransformEnemy_[e].translation_.z);
					// 衝突したら
					if (ex < 1 && ez < 1) {
						// 存在しない
						beamFlag_[b] = 0;
						enemyFlag_[e] = 2;
						gameScore_ += 1;
						enemyJumpSpeed_[e] = 1;

						// バレットヒットSE
						audio_->PlayWave(soundDataHandleEnemyHitSE_);
					}
				}
			}
		}
	}
}

// 描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// 各シーンの背景2D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DBack(); // ゲームプレイ2D背景表示
		break;
	case 2:
		GamePlayDraw2DBack(); // ゲームプレイ2D背景表示
		break;
	case 3:
		GamePlayDraw2DBack(); // ゲームプレイ2D背景表示
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();

#pragma endregion

// ゲームプレイ表示3D
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 各ステージの3D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw3D();
		break;
	case 2:
		GamePlayDraw3D();
		break;
	case 3:
		GamePlayDraw3D();
		break;
	}

	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	debugText_->DrawAll();

	// 各シーンの近景2D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear(); // ゲームプレイ2D近景表示
		break;
	case 1:
		TitleDraw2DNear(); // タイトル2D表示
		break;
	case 2:
		GamePlayDraw2DNear(); // ゲームプレイ2D近景表示
		GameOverDraw2DNear(); // ゲームオーバー
		break;
	case 3:
		GamePlayDraw2DNear(); // ゲームプレイ2D近景表示
		char str[100];
		sprintf_s(str, "GAME CREAL");
		debugText_->Print(str, 550, 200, 2);// ゲームクリア表示
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

// ゲームプレイ表示3D
void GameScene::GamePlayDraw3D() {
	// ステージ
	for (int s = 0; s < 20; s++) {
		modelStage_->Draw(worldTransformStage_[s], viewProjection_, textureHandleStage_);
	}

	// プレイヤー
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}

	// ビーム
	for (int b = 0; b < 10; b++) {
		if (beamFlag_[b] == 1) {
			modelBeam_->Draw(worldTransformBeam_[b], viewProjection_, textureHandleBeam_);
		}
	}

	// 敵
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[e] >= 1) {
			modelEnemy_->Draw(worldTransformEnemy_[e], viewProjection_, textureHandleEnemy_);
		}
	}
}

// ゲームプレイ表示2D背景
void GameScene::GamePlayDraw2DBack() {
	// 背景
	spriteBG_->Draw();
}

// ゲームプレイ表示2D近景
void GameScene::GamePlayDraw2DNear() {

	DrawScore();
	DrawLife();
}

// ゲームプレイ初期化
void GameScene::GamePlayStart() {
	gameScore_ = 0;
	playerLife_ = 3;
	gameTimer_ = 0;
	playerTimer_ = 0;
	for (int e = 0; e < 10; e++) {
		enemyFlag_[e] = 0;
	}
	for (int b = 0; b < 10; b++) {
		beamFlag_[b] = 0;
	}
	worldTransformPlayer_.translation_.x = 0;
	PlayerUpdate();
}

void GameScene::DrawScore() {
	// 各所の値を取り出す
	int eachNumber[5] = {};
	int number = gameScore_;

	int keta = 10000;
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta;
		number = number % keta;
		keta = keta / 10;
	}

	// 各桁の数値を描画
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}

	spriteScore_->Draw();

}

void GameScene::DrawLife() {
	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i]->Draw();
	}
}