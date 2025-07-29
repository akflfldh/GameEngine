#include "Utility.h"
#include<unicode/ustring.h>
#include<unicode/unistr.h>
#include<unicode/ucnv.h>
#include<unicode/errorcode.h>
#include<vector>
#include"uuid.h"

std::string CoreUtility::Utility::ConvertToString(const std::wstring& wstr, bool unicode)
{
    icu::UnicodeString u16String = wstr.c_str();
    std::string str;
    u16String.toUTF8String(str);
    return str;
}

std::wstring CoreUtility::Utility::ConvertToWString(const std::string& str, bool unicode)
{
    icu::UnicodeString u16String = icu::UnicodeString::fromUTF8(str);

    if (sizeof(wchar_t) == 2)
    {
        std::wstring wstr;
        size_t length = u16String.length();
        wstr.resize(length);

        u16String.extract(0, u16String.length(), icu::Char16Ptr(wstr.data()), 0);
        
        return wstr;
    }
    else if (sizeof(wchar_t) == 4)
    {
        std::vector<UChar32> uchar32Buffer(u16String.length() + 1);
        icu::ErrorCode errorCode;
        int32_t length = u16String.toUTF32(uchar32Buffer.data(), (int32_t)uchar32Buffer.size(), errorCode);
        if (!U_SUCCESS(errorCode))
        {
            //log
            return L"";
        }

        std::wstring wstr(reinterpret_cast<wchar_t*>(uchar32Buffer.data()), length);

        return wstr;

    }

    return L"";
}


void CoreUtility::Utility::ConvertUpperCase(std::string& oStr)
{

    icu::UnicodeString icuString = icu::UnicodeString::fromUTF8(oStr);
    icuString.toUpper();

    icuString.toUTF8String(oStr);
 
}

void CoreUtility::Utility::ConvertLowerCase(std::string& oStr)
{
    icu::UnicodeString icuString = icu::UnicodeString::fromUTF8(oStr);
    icuString.toLower();

    icuString.toUTF8String(oStr);

}



std::string CoreUtility::Utility::GetFileNameFromPath(const std::string& path, bool removeExtension )
{

    size_t slashPos = path.find_last_of('/');
    size_t backSlashPos = path.find_last_of('\\');

 

    std::size_t pos = std::max(slashPos, backSlashPos);
  
    if (slashPos == std::string::npos)
    {
        pos = backSlashPos;
    }
    else if (backSlashPos == std::string::npos)
    {
        pos = slashPos;
    }


    std::string fileName = (pos == std::string::npos) ? path : path.substr(pos + 1);
   
    if (removeExtension)
    {
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos)
        {
            fileName = fileName.substr(0, dotPos);
        }
    }
    

    return fileName;
}

std::string CoreUtility::Utility::GetExtensionFromPath(const std::string& path)
{

    size_t dotPos = path.find_last_of('.');
    size_t backSlashPos = path.find_last_of('\\');
    size_t slashPos = path.find_last_of('/');



    if (dotPos == std::string::npos)
        return "";

    if (dotPos < std::max(backSlashPos,slashPos))
    {
        //올바르지못하다.
        return "";
    }

    return  path.substr(dotPos + 1);
}

std::string CoreUtility::Utility::GetParentFolderPathFromPath(const std::string& path)
{


    size_t slashPos = path.find_last_of('/');
    size_t backSlashPos = path.find_last_of('\\');



    std::size_t pos = std::max(slashPos, backSlashPos);

    if (slashPos == std::string::npos)
    {
        pos = backSlashPos;
    }
    else if (backSlashPos == std::string::npos)
    {
        pos = slashPos;
    }


    std::string fileName = (pos == std::string::npos) ? path : path.substr( 0,pos);


    return fileName;


  
}

std::vector<std::string> CoreUtility::Utility::Split(const std::string& str, char delimiter)
{

    std::vector<std::string> tokenVector;

    std::stringstream ss(str);


    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        // 빈토큰 /folder1//folder의경우 건너띈다. 
        if (!token.empty())
        {
            tokenVector.push_back(std::move(token));
        }


    }


    return tokenVector;
}



std::string CoreUtility::Utility::MakeUniqueName(const std::string& baseName)
{

  


    static std::mt19937 generator([](){
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data{};
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());
        return std::mt19937(seq);
        }());

    static uuids::uuid_random_generator uuidGenerator(generator);

    uuids::uuid const id = uuidGenerator();
    std::string idStr =uuids::to_string(id);

    return baseName+"_"+idStr;
}


