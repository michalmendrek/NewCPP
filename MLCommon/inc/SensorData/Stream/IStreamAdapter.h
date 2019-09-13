#ifndef ISTREAMADAPTER_H
#define ISTREAMADAPTER_H

namespace ml {

class IStreamAdapterListener;

class IStreamAdapter
{
public:

    IStreamAdapter() = default;
    virtual ~IStreamAdapter() = default;

    virtual void Init() = 0;

    virtual void Shutdown() = 0;

    virtual void Start() = 0;

    virtual void Stop() = 0;

    void RegisterIStreamAdapterListener(IStreamAdapterListener* listener)
    {
        mListener = listener;
    }

protected:
    IStreamAdapterListener *mListener = nullptr;

private:
    IStreamAdapter(const IStreamAdapter&) = delete;
    IStreamAdapter& operator =(const IStreamAdapter&) = delete;
};

} // namespace - ml

#endif // ! ISTREAMADAPTER_H
