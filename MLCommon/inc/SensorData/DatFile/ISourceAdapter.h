#ifndef ISOURCEADAPTER_H
#define ISOURCEADAPTER_H

#include "ISourceAdapterListener.h"

namespace ml {

class ISourceAdapter
{
public:

    ISourceAdapter() = default;
    virtual ~ISourceAdapter() = default;

    virtual void Init() = 0;

    virtual void Shutdown() = 0;

    virtual void Start() = 0;

    virtual void Stop() = 0;

    void RegisterISourceAdapterListener(ISourceAdapterListener* listener)
    {
        mListener = listener;
    }

protected:
    ISourceAdapterListener *mListener = nullptr;

private:
    ISourceAdapter(const ISourceAdapter&) = delete;
    ISourceAdapter& operator =(const ISourceAdapter&) = delete;
};

} // namespace - ml

#endif // ! ISOURCEADAPTER_H
