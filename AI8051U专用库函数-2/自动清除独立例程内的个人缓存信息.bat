@echo off
setlocal enabledelayedexpansion

REM ����Ŀ���ļ���
set "target_dir=%~dp0��������"

REM �����������ʾ�ű���ʼ��Ϣ
echo ========================================
echo ��ʼ����Ŀ���ļ���: "%target_dir%"
echo ========================================
echo ���ڳ�ʼ�������Ժ�...
echo.

REM ���Ŀ���ļ����Ƿ����
if not exist "%target_dir%" (
    echo ����Ŀ���ļ��� "%target_dir%" �����ڣ�
    pause
    exit /b
)

REM ��ʼ��������
set /a file_count=0
set /a deleted_count=0

REM �����������ʾ�û��ű���������
echo ����ɨ���ļ����������Ժ�...
echo ����ָʾ��[ÿ�����ʾһ���ļ�������]
echo.

REM ����Ŀ���ļ����ڵ������ļ�
for /r "%target_dir%" %%f in (*) do (
    set /a file_count+=1
    set "file_path=%%f"
    set "file_ext=%%~xf"
    set "file_dir=%%~dpf"

    REM ��ʾ����ָʾ��ÿ���ļ�����ʱ���һ���㣩
    set /p =.<nul

    REM ����ļ��Ƿ���.vscode�ļ�����
    echo !file_dir! | findstr /i /c:".vscode" >nul
    if !errorlevel! neq 0 (
        REM ����ļ���չ������.hex��.c��.h��.lib��.uvproj����ɾ��
        if /i not "!file_ext!"==".hex" (
            if /i not "!file_ext!"==".c" (
                if /i not "!file_ext!"==".h" (
                    if /i not "!file_ext!"==".lib" (
                        if /i not "!file_ext!"==".uvproj" (
                            if /i not "!file_ext!"==".asm" (
 				if /i not "!file_ext!"==".exe" (
				    if /i not "!file_ext!"==".png" (
                                REM ɾ���ļ�������ɾ�������������
                                del "%%f" >nul 2>&1
                                set /a deleted_count+=1
				    )
				)
                            )
                        )
                    )
                )
            )
        )
    )
)

REM �����������ʾ������
echo.
echo ========================================
echo ������ɣ�
echo ���ļ���: !file_count!
echo ��ɾ���ļ���: !deleted_count!
if !deleted_count! equ 0 (
    echo û����Ҫɾ�����ļ���
) else (
    echo ��ɾ���� !deleted_count! ���ļ���
)
echo ========================================

pause