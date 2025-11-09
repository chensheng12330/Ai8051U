@echo off
setlocal enabledelayedexpansion

REM 设置目标文件夹
set "target_dir=%~dp0独立例程"

REM 美化输出：显示脚本开始信息
echo ========================================
echo 开始清理目标文件夹: "%target_dir%"
echo ========================================
echo 正在初始化，请稍候...
echo.

REM 检查目标文件夹是否存在
if not exist "%target_dir%" (
    echo 错误：目标文件夹 "%target_dir%" 不存在！
    pause
    exit /b
)

REM 初始化计数器
set /a file_count=0
set /a deleted_count=0

REM 美化输出：提示用户脚本正在运行
echo 正在扫描文件并清理，请稍候...
echo 进度指示：[每个点表示一个文件被处理]
echo.

REM 遍历目标文件夹内的所有文件
for /r "%target_dir%" %%f in (*) do (
    set /a file_count+=1
    set "file_path=%%f"
    set "file_ext=%%~xf"
    set "file_dir=%%~dpf"

    REM 显示进度指示（每个文件处理时输出一个点）
    set /p =.<nul

    REM 检查文件是否在.vscode文件夹内
    echo !file_dir! | findstr /i /c:".vscode" >nul
    if !errorlevel! neq 0 (
        REM 如果文件扩展名不是.hex、.c、.h、.lib、.uvproj，则删除
        if /i not "!file_ext!"==".hex" (
            if /i not "!file_ext!"==".c" (
                if /i not "!file_ext!"==".h" (
                    if /i not "!file_ext!"==".lib" (
                        if /i not "!file_ext!"==".uvproj" (
                            if /i not "!file_ext!"==".asm" (
 				if /i not "!file_ext!"==".exe" (
				    if /i not "!file_ext!"==".png" (
                                REM 删除文件（隐藏删除操作的输出）
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

REM 美化输出：显示处理结果
echo.
echo ========================================
echo 清理完成！
echo 总文件数: !file_count!
echo 已删除文件数: !deleted_count!
if !deleted_count! equ 0 (
    echo 没有需要删除的文件。
) else (
    echo 共删除了 !deleted_count! 个文件。
)
echo ========================================

pause