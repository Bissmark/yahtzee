#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the applications title bar
static TCHAR szTitle[] = _T("Yahtzee");

// Stored instance handle for us in Win32 API calls such as FindResource
HINSTANCE hInst;

// Forward declarations of functions in this code module
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define ID_BUTTON 1

static int potentialScore = 0;
static int diceFaces[5];
static int scores[2][16] = { 0 };

HWND hButton;

// Call this function at the beginning of your program
void CreateConsole()
{
	if (AllocConsole()) {
		FILE* stream;
		// Redirect stdout
		freopen_s(&stream, "CONOUT$", "w", stdout);
		std::cout.clear();
		std::wcout.clear();
		std::cerr.clear();
		std::wcerr.clear();
		std::cin.clear();
		std::wcin.clear();
	}
}

// Define number of attempts to roll the dice
int rollAttempts = 2;
int currentPlayer = 1;

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCMDShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBoxW(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Yahtzee"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindowEx explained:
	// WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application dows not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		2000, 1250,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Yahtzee"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, nCMDShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int) msg.wParam;
}

void DrawDiceFace(HDC hdc, int left, int top, int size, int face) {
	int dotRadius = size / 10; // Adjust this value as needed
	int dotDiameter = 2 * dotRadius;
	int mid = size / 2;
	int quarter = size / 4;
	int threeQuarters = 3 * size / 4;

	// Draw the dots
	switch (face) {
	case 6:
		// Draw the middle two dots
		Ellipse(hdc, left + quarter - dotRadius, top + mid - dotRadius, left + quarter + dotRadius, top + mid + dotRadius);
		Ellipse(hdc, left + threeQuarters - dotRadius, top + mid - dotRadius, left + threeQuarters + dotRadius, top + mid + dotRadius);
		// Fall through to case 4 to draw the remaining dots
	case 4:
		// Draw the top and bottom middle dots
		Ellipse(hdc, left + quarter - dotRadius, top + quarter - dotRadius, left + quarter + dotRadius, top + quarter + dotRadius);
		Ellipse(hdc, left + threeQuarters - dotRadius, top + quarter - dotRadius, left + threeQuarters + dotRadius, top + quarter + dotRadius);
		Ellipse(hdc, left + quarter - dotRadius, top + threeQuarters - dotRadius, left + quarter + dotRadius, top + threeQuarters + dotRadius);
		Ellipse(hdc, left + threeQuarters - dotRadius, top + threeQuarters - dotRadius, left + threeQuarters + dotRadius, top + threeQuarters + dotRadius);
		break;
	case 5:
		// Draw the middle dot
		Ellipse(hdc, left + mid - dotRadius, top + mid - dotRadius, left + mid + dotRadius, top + mid + dotRadius);
		// Draw the top and bottom middle dots
		Ellipse(hdc, left + quarter - dotRadius, top + quarter - dotRadius, left + quarter + dotRadius, top + quarter + dotRadius);
		Ellipse(hdc, left + threeQuarters - dotRadius, top + quarter - dotRadius, left + threeQuarters + dotRadius, top + quarter + dotRadius);
		Ellipse(hdc, left + quarter - dotRadius, top + threeQuarters - dotRadius, left + quarter + dotRadius, top + threeQuarters + dotRadius);
		Ellipse(hdc, left + threeQuarters - dotRadius, top + threeQuarters - dotRadius, left + threeQuarters + dotRadius, top + threeQuarters + dotRadius);
		break;
	case 3:
		// Draw the top right and bottom left dots
		Ellipse(hdc, left + threeQuarters - dotRadius, top + quarter - dotRadius, left + threeQuarters + dotRadius, top + quarter + dotRadius);
		Ellipse(hdc, left + quarter - dotRadius, top + threeQuarters - dotRadius, left + quarter + dotRadius, top + threeQuarters + dotRadius);
		// Fall through to case 1 to draw the remaining dot
	case 1:
		// Draw the center dot
		Ellipse(hdc, left + mid - dotRadius, top + mid - dotRadius, left + mid + dotRadius, top + mid + dotRadius);
		break;
	case 2:
		// Draw the top left and bottom right dots
		Ellipse(hdc, left + quarter - dotRadius, top + quarter - dotRadius, left + quarter + dotRadius, top + quarter + dotRadius);
		Ellipse(hdc, left + threeQuarters - dotRadius, top + threeQuarters - dotRadius, left + threeQuarters + dotRadius, top + threeQuarters + dotRadius);
		break;
	}
}

void onScoreTableClick(int row) {
	// I want to check if there is a 1, 2, 3, 4, 5 or 2, 3, 4, 5, 6 as the dice faces, if so, the potential score should be 40
	// It shouldnt matter which dice faces are any of the numbers above
	potentialScore = 0;

	int sortedDiceFaces[5];
	for (int i = 0; i < 5; ++i) {
		sortedDiceFaces[i] = diceFaces[i];
	}
	std::sort(sortedDiceFaces, sortedDiceFaces + 5);

	// Removing duplicates
	std::vector<int> uniqueFaces;
	uniqueFaces.push_back(sortedDiceFaces[0]);
	for (int i = 1; i < 5; ++i) {
		if (sortedDiceFaces[i] != sortedDiceFaces[i - 1]) {
			uniqueFaces.push_back(sortedDiceFaces[i]);
		}
	}

	// Ones, Twos, Threes, Fours, Fives, Sixes
	if (row >= 1 && row <= 6) {
		for (int i = 0; i < 5; ++i) {
			if (diceFaces[i] == row) {
				potentialScore += row;
			}
		}
	}
	
	// Small Straight
	// Three of a kind
	if (row == 9 && (sortedDiceFaces[0] == sortedDiceFaces[2] || sortedDiceFaces[1] == sortedDiceFaces[3] || sortedDiceFaces[2] == sortedDiceFaces[4])) {
		potentialScore = sortedDiceFaces[0] + sortedDiceFaces[1] + sortedDiceFaces[2] + sortedDiceFaces[3] + sortedDiceFaces[4];
	}

	// Four of a kind
	if (row == 10 && (sortedDiceFaces[0] == sortedDiceFaces[3] || sortedDiceFaces[1] == sortedDiceFaces[4])) {
		potentialScore = sortedDiceFaces[0] + sortedDiceFaces[1] + sortedDiceFaces[2] + sortedDiceFaces[3] + sortedDiceFaces[4];
	}

	// Full House
	if (row == 11 && ((sortedDiceFaces[0] == sortedDiceFaces[1] && sortedDiceFaces[2] == sortedDiceFaces[3] && sortedDiceFaces[3] == sortedDiceFaces[4]) ||
		(sortedDiceFaces[0] == sortedDiceFaces[1] && sortedDiceFaces[1] == sortedDiceFaces[2] && sortedDiceFaces[3] == sortedDiceFaces[4]))) {
		potentialScore = 25;
	}

	if (row == 12) {
		if ((uniqueFaces.size() >= 4 && uniqueFaces[0] + 1 == uniqueFaces[1] && uniqueFaces[1] + 1 == uniqueFaces[2] && uniqueFaces[2] + 1 == uniqueFaces[3]) ||
			(uniqueFaces.size() == 5 && uniqueFaces[1] + 1 == uniqueFaces[2] && uniqueFaces[2] + 1 == uniqueFaces[3] && uniqueFaces[3] + 1 == uniqueFaces[4])) {
			potentialScore = 30;
		}
	}

	// Large Straight
	if (row == 13 && (sortedDiceFaces[0] == 1 && sortedDiceFaces[1] == 2 && sortedDiceFaces[2] == 3 && sortedDiceFaces[3] == 4 && sortedDiceFaces[4] == 5) ||
		(sortedDiceFaces[0] == 2 && sortedDiceFaces[1] == 3 && sortedDiceFaces[2] == 4 && sortedDiceFaces[3] == 5 && sortedDiceFaces[4] == 6)) {
		potentialScore = 40;
	}

	// Chance
	if (row == 14) {
		for (int i = 0; i < 5; ++i) {
			potentialScore += diceFaces[i];
		}
	}

	// Yahtzee
	if (row == 15 && sortedDiceFaces[0] == sortedDiceFaces[4]) {
		potentialScore = 50;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH hBrush;
	RECT rect;

	// Get client area dimensions
	GetClientRect(hWnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	// Define dice size and spacing
	const int diceSize = 50;
	const int numDice = 5;
	const int spacing = 20; // Space between dice

	// Calculate total width of all dice including spacing
	int totalWidth = numDice * diceSize + (numDice - 1) * spacing;

	// Calculate starting position to center the dice
	int startX = (windowWidth - totalWidth) / 2;
	int startY = (windowHeight - diceSize) / 2;

	static bool heldDice[5] = { false };

	static std::wstring players[2] = { L"CJ", L"Bob" };
	static std::wstring typesOfScoring[16] = { L"Ones", L"Twos", L"Threes", L"Fours", L"Fives", L"Sixes", L"Sum", L"Bonus", L"Three of a kind", L"Four of a kind", L"Full House", L"Small Straight", L"Large Straight", L"Chance", L"YAHZTEE", L"Total Score"};

	static int player1Score = 0;
	static int player2Score = 0;

	static int totalScorePlayer1 = 0;
	static int totalScorePlayer2 = 0;

	// Define dimensions of the scoring table
	static int tableLeft = startX + 1500;
	static int tableTop = 100;
	static int tableRight = tableLeft + 500;
	static int tableBottom = tableTop + 1000;
	const int numRows = 17;

	// Initialize prevPlayerName to an empty string or any value that is not a valid player name
	static std::wstring prevPlayerName = L"";


	switch (message)
	{
	case WM_CREATE:
	{
		srand((unsigned int)time(NULL));

		for (int i = 0; i < 5; ++i) {
			diceFaces[i] = (rand() % 6) + 1;
			//std::cout << "diceFaces[" << i << "] = " << diceFaces[i] << std::endl;
		}

		hButton = CreateWindow(
			L"BUTTON",
			L"Roll Dice",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			10,
			10,
			100,
			30,
			hWnd,
			(HMENU)ID_BUTTON,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		// Check if any die was clicked
		for (int i = 0; i < numDice; ++i) {
			int left = startX + i * (diceSize + spacing);
			int top = startY;
			int right = left + diceSize;
			int bottom = top + diceSize;

			if (x >= left && x <= right && y >= top && y <= bottom) {
				heldDice[i] = !heldDice[i]; // Toggle the state of the die

				InvalidateRect(hWnd, NULL, TRUE); // Redraw the window
				UpdateWindow(hWnd); // Ensure immediate update
				break; // No need to check other dice
			}
		}

		// Check if the score table was clicked
		if (x >= tableLeft && x <= tableRight) {
			int rowHeight = (tableBottom - tableTop) / numRows;
			int row = (y - tableTop) / rowHeight;

			if (row == 7 || row == 8 || row == 16) {
				break;
			}

			// Calculate the column width
			const int numColumns = 3;
			int colWidth = (tableRight - tableLeft) / numColumns;

			// Calculate the column of the click
			int col = (x - tableLeft) / colWidth;

			// Prevent player 1 from clicking on player 2's column and vice versa
			if ((currentPlayer == 1 && col != 1) || (currentPlayer == 2 && col != 2)) {
				break;
			}

			if (row >= 0 && row < numRows) {
				onScoreTableClick(row);

				// Store the potential score in the corresponding element of the scores array
				scores[currentPlayer - 1][row] = potentialScore;

				currentPlayer = (currentPlayer % 2) + 1;
				prevPlayerName = players[currentPlayer - 1];
				if (rollAttempts == 0) {
					rollAttempts = 2; // Reset rollAttempts for the next player
					EnableWindow(hButton, TRUE);

					for (int i = 0; i < numDice; i++) {
						heldDice[i] = false;
						diceFaces[i] = (rand() % 6) + 1;
					}
					InvalidateRect(hWnd, NULL, TRUE); // Redraw the window

					potentialScore = 0;
					totalScorePlayer1 = 0;
					totalScorePlayer2 = 0;
				}
			}
		}

		break;
	}

	case WM_ERASEBKGND:
		{
			hdc = (HDC)wParam;
			GetClientRect(hWnd, &rect);
			hBrush = CreateSolidBrush(RGB(100, 100, 250)); // Custom background color
			FillRect(hdc, &rect, hBrush);
			DeleteObject(hBrush);
			return 1;
		}

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		// Draw squares representing dice
		for (int i = 0; i < numDice; ++i) {
			int left = startX + i * (diceSize + spacing);
			int top = startY;
			int right = left + diceSize;
			int bottom = top + diceSize;
			
			// If the die is held, draw it with a different color or style
			if (heldDice[i]) {
				// For example, draw a red outline around the die
				HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
				HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // Create a white brush
				HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush); // Select the white brush into the device context
				SelectObject(hdc, GetStockObject(NULL_BRUSH));
				Rectangle(hdc, left, top, right, bottom);
				SelectObject(hdc, hOldPen); // Restore the original pen
				SelectObject(hdc, hOldBrush); // Restore the original brush
				DeleteObject(hPen);
				DeleteObject(hBrush); // Delete the brush
			}

			// Draw the dice face with a white background
			HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hWhiteBrush);
			Rectangle(hdc, left, top, right, bottom);
			DrawDiceFace(hdc, left, top, diceSize, diceFaces[i]);
			SelectObject(hdc, hOldBrush);
			DeleteObject(hWhiteBrush);
		}

		

		// Draw table boundary
		Rectangle(hdc, tableLeft, tableTop, tableRight, tableBottom);

		const int numColumns = 3;
		int rowHeight = (tableBottom - tableTop) / numRows;
		int colWidth = (tableRight - tableLeft) / numColumns;

		// Draw the table horizontal lines for the rows
		for (int i = 1; i < numRows; i++) {
			int y = tableTop + i * rowHeight;
			std::wstringstream ss1, ss2;
			ss1 << scores[0][i];
			ss2 << scores[1][i];
			std::wstring scoreStr1 = ss1.str();
			std::wstring scoreStr2 = ss2.str();
			TextOut(hdc, tableLeft + 180, y + 20, scoreStr1.c_str(), static_cast<int>(scoreStr1.length()));
			TextOut(hdc, tableLeft + 345, y + 20, scoreStr2.c_str(), static_cast<int>(scoreStr2.length()));  // some error here causes large number to be displayed
			MoveToEx(hdc, tableLeft, y, NULL);
			LineTo(hdc, tableRight, y);

		}

		// Draw the table vertical lines for the columns
		for (int i = 1; i < numColumns; ++i) {
			int x = tableLeft + i * colWidth;
			MoveToEx(hdc, x, tableTop, NULL);
			LineTo(hdc, x, tableBottom);
		}

		// Draw the scoring text from the array into the table
		for (int i = 0; i < 16; ++i) {
			int y = tableTop + (i + 1) * rowHeight + (rowHeight - 20) / 2;
			TextOut(hdc, tableLeft + 10, y, typesOfScoring[i].c_str(), typesOfScoring[i].length());
		}

		std::wstring attemptsText = L"Roll Attempts Left: " + std::to_wstring(rollAttempts);
		TextOut(hdc, 10, 50, attemptsText.c_str(), attemptsText.length());

		std::wstring currentPlayerText = L"Current Player: " + players[currentPlayer - 1];
		TextOut(hdc, 10, 70, currentPlayerText.c_str(), currentPlayerText.length());

		std::wstring potentialScoreText = L"Potential Score: " + std::to_wstring(potentialScore);
		TextOut(hdc, 10, 110, potentialScoreText.c_str(), potentialScoreText.length());

		for (int i = 0; i < 16; ++i) {
			totalScorePlayer1 += scores[0][i];
		}

		std::wstring totalScorePlayer1Text = L"Total Score Player 1 " + std::to_wstring(totalScorePlayer1);
		TextOut(hdc, 10, 150, totalScorePlayer1Text.c_str(), totalScorePlayer1Text.length());

		//scores[0][16] = totalScorePlayer1;

		for (int i = 0; i < 16; ++i) {
			totalScorePlayer2 += scores[1][i];
		}

		std::wstring totalScorePlayer2Text = L"Total Score Player 2 " + std::to_wstring(totalScorePlayer2);
		TextOut(hdc, 10, 190, totalScorePlayer2Text.c_str(), totalScorePlayer2Text.length());

		//scores[1][16] = totalScorePlayer2;

		// Draw player names in the first row of the table
		SetBkMode(hdc, TRANSPARENT);
		for (int i = 0; i < 2; ++i) {
			int x = tableLeft + (i + 1) * colWidth;
			int y = tableTop;
			RECT playerRect = { x, y, x + colWidth, y + rowHeight };

			// If this is the current player, fill the box with a different color
			if (i + 1 == currentPlayer) {
				HBRUSH hBrush = CreateSolidBrush(RGB(200, 0, 200)); // Light gray color
				FillRect(hdc, &playerRect, hBrush);
				DeleteObject(hBrush);
			}

			TextOut(hdc, x + 20, y + (rowHeight - 20) / 2, players[i].c_str(), players[i].length());
		};

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_BUTTON:
			// Decrement rollAttempts
			
			rollAttempts--;

			// Generate new random dice faces
			for (int i = 0; i < numDice; ++i) {
				if (!heldDice[i]) { // Only roll the die if it's not held
					diceFaces[i] = (rand() % 6) + 1;
				}
			}

			if (rollAttempts == 0) {
				EnableWindow(hButton, FALSE);
			}
			
			// Redraw the window to update the roll attempts and score text
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}