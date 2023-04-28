#include "GameScene.h"
#include "TextureManager.h"
#include "MathUtilityForText.h"
#include <cassert>

//コントストラクタ
GameScene::GameScene() {}

//デストラクタ
GameScene::~GameScene() { 
	delete spriteBG_;	//BG
	delete modelStage_;	//ステージ
	delete modelPlayer_;// プレイヤー
	delete modelBeam_;	//ビーム
}

//初期化
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

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	// ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};

	// 変数行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);

	// 変更行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.1f, 0.1f, 0.1f};
	worldTransformBeam_.Initialize();
}

//更新
void GameScene::Update() { 
	PlayerUpdate();
	BeamUpdate();
	BeamBorn();
}

//----------------------------------------------
// プレイヤー
//----------------------------------------------

//プレイヤー更新
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

	//変数行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, 
		worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	//変数行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
}

//----------------------------------------------
// ビーム
//----------------------------------------------

//ビーム更新
void GameScene::BeamUpdate() {
	//移動
	BeamMove();
}

//ビーム移動
void GameScene::BeamMove() {
	worldTransformBeam_.rotation_.x += 0.1f; 
}

//ビーム発生 (発射)
void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE)) {
		beamFlag_ = 1;
		if (worldTransformPlayer_.translation_.z > 40) {
			beamFlag_ = 0;
		}
	}
}


//描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
	spriteBG_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	//ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);

	//プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	//ビーム
	if (beamFlag_ == 1) {
		modelBeam_->Draw(worldTransformPlayer_, viewProjection_, textureHandleBeam_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
