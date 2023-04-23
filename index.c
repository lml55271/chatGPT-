#include <windows.h>
#include <stdio.h>

#define ID_BTN_SORT 1001
#define ID_BTN_RESET 1002
#define ID_COMBO_SORT 1003
#define ID_TIMER_SORT 1004

#define MAX_NUMS 100
#define TIMER_INTERVAL 100

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszClassName = "SortWndClass";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow("SortWndClass", "Sorting Algorithms", WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

int nums[MAX_NUMS] = { 0 };
int numCount = 0;
int curIndex = 0;
int sortType = 0;
BOOL isSorting = FALSE;

void resetNums()
{
    numCount = 0;
    curIndex = 0;
    isSorting = FALSE;
    InvalidateRect(NULL, NULL, TRUE);
}

void sortNums()
{
    isSorting = TRUE;
    SetTimer(NULL, ID_TIMER_SORT, TIMER_INTERVAL, NULL);
}

void updateNums(HDC hdc)
{
    RECT rect;
    GetClientRect(GetParent(hdc), &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int xStep = width / numCount;
    int yStep = height / MAX_NUMS;

    for (int i = 0; i < numCount; i++)
    {
        RECT numRect = { i * xStep, height - nums[i] * yStep, (i + 1) * xStep, height };
        if (i == curIndex)
        {
            FillRect(hdc, &numRect, (HBRUSH)(COLOR_HIGHLIGHT));
        }
        else
        {
            FillRect(hdc, &numRect, (HBRUSH)(COLOR_WINDOW));
        }
    }
}

void updateSort()
{
    switch (sortType)
    {
    case 0: // Insertion Sort
    {
        if (curIndex < numCount)
        {
            int key = nums[curIndex];
            int j = curIndex - 1;
            while (j >= 0 && nums[j] > key)
            {
                nums[j + 1] = nums[j];
                j--;
            }
            nums[j + 1] = key;
        }
        break;
    }
    case 1: // Selection Sort
    {
        if (curIndex < numCount)
        {
            int minIndex = curIndex;
            for (int i = curIndex + 1; i < numCount; i++)
            {
                if (nums[i] < nums[minIndex])
                {
                    minIndex = i;
                }
            }
            int temp = nums[curIndex];
            nums[curIndex] = nums[minIndex];
            nums[minIndex] = temp;
        }
        break;
    }
    case 2: // Bubble Sort
    {
        if (curIndex < numCount)
        {
            for (int i = 0; i < numCount - curIndex - 1; i++)
            {
                if (nums[i] > nums[i + 1])
                {
                    int temp = nums[i];
                    nums[i] = nums[i + 1];
                    nums[i + 1] = temp;
                }
            }
        }
        break;
    }
    case 3: // Quick Sort
    {
        // TODO: Implement Quick Sort
        break;
    }
    }
    curIndex++;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HWND btnSort = CreateWindow("BUTTON", "Sort", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 80, 30, hWnd, (HMENU)ID_BTN_SORT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        HWND btnReset = CreateWindow("BUTTON", "Reset", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 10, 80, 30, hWnd, (HMENU)ID_BTN_RESET, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        HWND comboSort = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 190, 10, 120, 30, hWnd, (HMENU)ID_COMBO_SORT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        SendMessage(comboSort, CB_ADDSTRING, 0, (LPARAM)"Insertion Sort");
        SendMessage(comboSort, CB_ADDSTRING, 0, (LPARAM)"Selection Sort");
        SendMessage(comboSort, CB_ADDSTRING, 0, (LPARAM)"Bubble Sort");
        SendMessage(comboSort, CB_ADDSTRING, 0, (LPARAM)"Quick Sort");
        SendMessage(comboSort, CB_SETCURSEL, 0, 0);

        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_BTN_SORT:
        {
            if (numCount > 0 && !isSorting)
            {
                sortType = SendMessage(GetDlgItem(hWnd, ID_COMBO_SORT), CB_GETCURSEL, 0, 0);
                sortNums();
            }
            break;
        }
        case ID_BTN_RESET:
        {
            resetNums();
            break;
        }
        case ID_COMBO_SORT:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                resetNums();
            }
            break;
        }
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        updateNums(hdc);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_TIMER:
    {
        if (wParam == ID_TIMER_SORT)
        {
            updateSort();
            InvalidateRect(hWnd, NULL, FALSE);
            if (curIndex >= numCount)
            {
                KillTimer(hWnd, ID_TIMER_SORT);
                isSorting = FALSE;
            }
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (!isSorting)
        {
            int xPos = LOWORD(lParam);
            int yPos = HIWORD(lParam);
            RECT rect;
            GetClientRect(hWnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            int xStep = width / numCount;
            int yStep = height / MAX_NUMS;
            int index = xPos / xStep;
            int num = (height - yPos) / yStep;
            if (index >= 0 && index < numCount && num >= 0 && num <= MAX_NUMS)
            {
                nums[index] = num;
                curIndex = 0;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }

    return 0;
}
