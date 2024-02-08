#include <sstream>

class Formatter final
{
public:
  Formatter() = default;
  ~Formatter() = default;

  template<typename Type>
  Formatter & operator<<(const Type & value)
  {
    stream_ << value;
    return *this;
  }

  std::string str() const { return stream_.str(); }
  operator std::string() const { return stream_.str(); }

  enum ConvertToString
  {
    to_str
  };
  std::string operator>>(ConvertToString) { return stream_.str(); }

private:
  std::stringstream stream_;

private:
  Formatter(const Formatter &) = delete;
  Formatter & operator=(Formatter &) = delete;
};
