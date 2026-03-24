#include <wx/regex.h>
#include "MarkdownWindow.h"

void MarkdownWindow::CalculateTypingStatistics(const wxString& input) {
    long totalChars = input.Length();
    wxRegEx reWord("\\w+"); 
    long wordCount = 0;
    size_t start = 0;

    while (start < input.length() && reWord.Matches(input.Mid(start))) {
        wordCount++;
        size_t matchStart, matchLen;
        reWord.GetMatch(&matchStart, &matchLen);
        start += (matchStart + matchLen);
    }

    wxString statusMsg = wxString::Format("%ld Words, %ld Characters", wordCount, totalChars);
    SetStatusText(statusMsg, 1);
}

void MarkdownWindow::OnTypingStatisticsTimer(wxTimerEvent& event)
{
  CalculateTypingStatistics(this->textCtrl->GetValue());
}
