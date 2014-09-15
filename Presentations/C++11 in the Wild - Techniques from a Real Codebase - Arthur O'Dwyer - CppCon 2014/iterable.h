template<class It>
class iterable
{
  It m_first, m_last;
public:
  iterable() = default;
  iterable(It first, It last) :
    m_first(first), m_last(last) {}
  It begin() const { return m_first; }
  It end() const { return m_last; }
};

template<class It>
static inline iterable<It> make_iterable(It a, It b)
{
  return iterable<It>(a, b);
}
