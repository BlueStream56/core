#include "svector.h"

template<class T>
SVector<T>::SVector(size_t reserve, T initValue) : array(reserve, initValue)
{
    init();
}

template<class T>
SVector<T>::SVector(bool needInit)
{
    isInit = needInit;
}

template<class T>
SVector<T>::SVector(size_t reserve) : array(reserve)
{
    init();
}

template<class T>
std::shared_ptr<T> SVector<T>::operator[](size_t pos) const
{
    if (!isInit || array.size() <= pos)
        return {};

    return array[pos];
}

template<class T>
void SVector<T>::push_back(T &&value)
{
    isInit = true;
    array.push_back(std::shared_ptr<T>(value));
}

template<class T>
void SVector<T>::push_back(const T &value)
{
    isInit = true;
    array.push_back(std::shared_ptr<T>(value));
}

template<class T>
std::shared_ptr<T> SVector<T>::back() const
{
    if (!isInit || array.empty())
        return {};

    return array.back();
}

template<class T>
std::shared_ptr<T> SVector<T>::front() const
{
    if (!isInit || array.empty())
        return {};

    return array.front();
}
