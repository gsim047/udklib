#include <Constants.au3>

#pragma compile(Icon, C:\Program Files (x86)\AutoIt3\Icons\au3.ico)

;
; AutoIt Version: 3.0
; Language:       English
; Platform:       Win x32/x64
; Author:         Georgiy Stepanov
;
; Script Function:
;   Auto scan with Epson Perfection 1670 scaner. Just press button on scaner face...
;


;HotKeySet("{ESC}", "_Terminate")

_Scan()


Func _Scan()
	Local $tit1 = "EPSON Perfection 1670"
	Local $tit2 = "Импортированные изображения и видео"
	Local $tit3 = "Импорт изображений и видео"
	While 1
		Local $hnd = Null

		WinWait($tit1, "")
		Sleep(100)
		WinActivate($tit1)
		Sleep(100)
		Send("{DOWN}{ENTER}")

		If $hnd <> Null Then
			If Not WinExists($hnd) Then
				$hnd = Null
			EndIf
		EndIf

		If $hnd = Null Then
			If WinExists($tit2) Then
				$hnd = WinGetHandle($tit2)
			EndIf
		EndIf

		WinWait($tit3)
		WinWaitClose($tit3)
		If $hnd = Null Then
			WinWait($tit2)
			$hnd = WinGetHandle($tit2)
		Else
			Sleep(100)
			While 1
				Local $lst = WinList()
				If $lst[0][0] > 1 Then
					For $i = 0 To $lst[0][0]
						If $lst[$i][0] = $tit2 And $lst[$i][1] <> $hnd Then
							WinClose($lst[$i][1])
						EndIf
					Next
					ExitLoop
				Else
					ContinueLoop
				EndIf
			WEnd
		EndIf

		Sleep(100)
		WinActivate($hnd)
		Sleep(100)
		Send("{F5}")

	  Sleep(2000)
   WEnd
EndFunc   ;==>_Scan

Func _Terminate()
    Exit
EndFunc   ;==>_Terminate

; Finished!
