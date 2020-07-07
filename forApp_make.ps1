# lib-doodleアプリ向け加工スクリプト
# 2020/06/24 T.Maki

# ルートフォルダは～\lib-doodle
# スクリプトファイルはルートフォルダに格納して実行する

# Gitクローンおよびlib-bleサブモジュールのアップデートは作業者が行う

# カレントフォルダのチェック
$strpath = (Convert-Path .)
$strwishend = "lib-doodle"

# スクリプトのあるフォルダを起点にする
if (!$PSScriptRoot.EndsWith($strwishend)) {
    Write-Host "スクリプトのあるフォルダが不正なので終了します。"
    Write-Host '～\';$strwishend;"に格納してください。"
    exit
}

# 成果物をリリース作業用フォルダにコピーする

# リリース作業用フォルダには3rdparty、bin、includeのサブフォルダがある
if (Test-Path ".\lib-doodle") {
    #フォルダが存在する場合
    Write-Host "リリース作業用フォルダが存在するので、再作成します。"
    Remove-Item -Force -Recurse -Path ".\lib-doodle"
} else {
    #フォルダが存在しない場合
    Write-Host "リリース作業用フォルダは存在しないので、作成します。"
} 
New-Item ".\lib-doodle" -ItemType Directory
New-Item ".\lib-doodle\bin" -ItemType Directory
New-Item ".\lib-doodle\bin\x64" -ItemType Directory
New-Item ".\lib-doodle\bin\x64\Debug" -ItemType Directory
New-Item ".\lib-doodle\bin\x64\Release" -ItemType Directory
New-Item ".\lib-doodle\include" -ItemType Directory
New-Item ".\lib-doodle\include\doodle" -ItemType Directory
New-Item ".\lib-doodle\src" -ItemType Directory
New-Item ".\lib-doodle\src\doodle" -ItemType Directory
New-Item ".\lib-doodle\3rdparty" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\camera" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\camera\include" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\camera\include\opencv2" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\camera\lib" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\mpjsdk" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\mpjsdk\include" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\picojson" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\picojson\include" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\SentechSDK" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\SentechSDK\include" -ItemType Directory
New-Item ".\lib-doodle\3rdparty\SentechSDK\lib" -ItemType Directory
New-Item ".\lib-doodle\lib-ble\bin" -ItemType Directory
New-Item ".\lib-doodle\lib-ble\include" -ItemType Directory

# 成果物をリリース作業用サブフォルダにコピー
robocopy .\ .\lib-doodle lib-doodle.sln
robocopy .\ .\lib-doodle lib-doodle.vcxproj
robocopy ".\3rdparty\camera\include" ".\lib-doodle\3rdparty\camera\include" /s /e
robocopy ".\3rdparty\camera\lib" ".\lib-doodle\3rdparty\camera\lib" /s /e
robocopy ".\3rdparty\mpjsdk\include" ".\lib-doodle\3rdparty\mpjsdk\include" /s /e
robocopy ".\3rdparty\picojson\include" ".\lib-doodle\3rdparty\picojson\include" /s /e
robocopy ".\3rdparty\SentechSDK\include" ".\lib-doodle\3rdparty\SentechSDK\include" /s /e
robocopy ".\3rdparty\SentechSDK\lib" ".\lib-doodle\3rdparty\SentechSDK\lib" /s /e
robocopy ".\bin\x64\Debug" ".\lib-doodle\bin\x64\debug" /s /e
robocopy ".\bin\x64\Release" ".\lib-doodle\bin\x64\Release" /s /e
robocopy ".\include\doodle" ".\lib-doodle\include\doodle" /s /e
robocopy ".\src\doodle" ".\lib-doodle\src\doodle" /s /e
robocopy ".\lib-ble\bin" ".\lib-doodle\lib-ble\bin" /s /e
robocopy ".\lib-ble\include" ".\lib-doodle\lib-ble\include" /s /e

# 作業用フォルダごと成果物をZipファイルに圧縮する
$filename = "lib-doodle.zip"
# ルートにあるリリースファイルを削除する
if (Test-Path ".\$($filename)") {
    # ルートにリリースファイルが存在する場合
    Write-Host "ルートにリリースファイルが存在するので、削除します。"
    Remove-Item -Path ".\$($filename)" -Force
}
Compress-Archive -Path .\lib-doodle -DestinationPath $filename -Force

# 後始末
Remove-Item -Force -Recurse -Path ".\lib-doodle"

# 完了
Write-Host "アプリ向け加工が完了しました。"
