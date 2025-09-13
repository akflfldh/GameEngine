#include <functional>

#include <CoreBase/CoreBaseDllMacro.h>

class COREBASE_API FName
{
  public:
    FName(const char *str = "");
    ~FName();

    explicit FName(size_t index);

    const char *GetStr() const;
    size_t GetIndex() const;

  private:
    size_t mIndex;
};

namespace std
{
template <> struct hash<FName>
{
    size_t operator()(const FName &name) const
    {
        return std::hash<size_t>()(name.GetIndex());
    }
};

} // namespace std
