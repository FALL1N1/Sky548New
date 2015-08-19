#define GOSSIP_ICON_TEXT(text, icon) IconText(text, icon)
#define GOSSIP_ICON_TEXT_RETURN(text) GOSSIP_ICON_TEXT(text, "Interface\\PaperDollInfoFrame\\UI-GearManager-Undo.blp")
#define GOSSIP_ICON_TEXT_RETURN_TO_MAIN_MENU GOSSIP_ICON_TEXT_RETURN("Return to the main menu.")
#define GOSSIP_ICON_TEXT_BROWSE_GOODS GOSSIP_ICON_TEXT("Browse my wares", "Interface\\Icons\\inv_misc_coin_06.blp")
#define BOSS_WHISPER_NOTIFY_ICON(text, icon) IconText(text, icon, 30, 0, 0, "   ")

inline std::string IconText(std::string text,
                            const char* icon, uint32 iconSize = 36,
                            int32 horizontalOffset = -13,
                            int32 verticalOffset = 1,
                            const char* offset = "")
{
    std::ostringstream os;

    os  << "|T" << icon << ':'
        << iconSize << ':' << iconSize << ':' << horizontalOffset << ':' << verticalOffset
        << "|t" << offset << text;

    return os.str();
}