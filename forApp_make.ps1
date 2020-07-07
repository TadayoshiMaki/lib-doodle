# lib-doodle�A�v���������H�X�N���v�g
# 2020/06/24 T.Maki

# ���[�g�t�H���_�́`\lib-doodle
# �X�N���v�g�t�@�C���̓��[�g�t�H���_�Ɋi�[���Ď��s����

# Git�N���[�������lib-ble�T�u���W���[���̃A�b�v�f�[�g�͍�Ǝ҂��s��

# �J�����g�t�H���_�̃`�F�b�N
$strpath = (Convert-Path .)
$strwishend = "lib-doodle"

# �X�N���v�g�̂���t�H���_���N�_�ɂ���
if (!$PSScriptRoot.EndsWith($strwishend)) {
    Write-Host "�X�N���v�g�̂���t�H���_���s���Ȃ̂ŏI�����܂��B"
    Write-Host '�`\';$strwishend;"�Ɋi�[���Ă��������B"
    exit
}

# ���ʕ��������[�X��Ɨp�t�H���_�ɃR�s�[����

# �����[�X��Ɨp�t�H���_�ɂ�3rdparty�Abin�Ainclude�̃T�u�t�H���_������
if (Test-Path ".\lib-doodle") {
    #�t�H���_�����݂���ꍇ
    Write-Host "�����[�X��Ɨp�t�H���_�����݂���̂ŁA�č쐬���܂��B"
    Remove-Item -Force -Recurse -Path ".\lib-doodle"
} else {
    #�t�H���_�����݂��Ȃ��ꍇ
    Write-Host "�����[�X��Ɨp�t�H���_�͑��݂��Ȃ��̂ŁA�쐬���܂��B"
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

# ���ʕ��������[�X��Ɨp�T�u�t�H���_�ɃR�s�[
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

# ��Ɨp�t�H���_���Ɛ��ʕ���Zip�t�@�C���Ɉ��k����
$filename = "lib-doodle.zip"
# ���[�g�ɂ��郊���[�X�t�@�C�����폜����
if (Test-Path ".\$($filename)") {
    # ���[�g�Ƀ����[�X�t�@�C�������݂���ꍇ
    Write-Host "���[�g�Ƀ����[�X�t�@�C�������݂���̂ŁA�폜���܂��B"
    Remove-Item -Path ".\$($filename)" -Force
}
Compress-Archive -Path .\lib-doodle -DestinationPath $filename -Force

# ��n��
Remove-Item -Force -Recurse -Path ".\lib-doodle"

# ����
Write-Host "�A�v���������H���������܂����B"
