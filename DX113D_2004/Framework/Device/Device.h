#pragma once

class RenderTarget;
class DepthStencil;

class Device : public Singleton<Device>
{
	
public:
	void CreateDeviceAndSwapchain();
	void CreateBackBuffer();

	void SetRenderTarget();
	void SetRenderTargetNullDSV();

	void InitRenderTargets(RenderTarget** renderTargets, int count);
	void SetRenderTargets();

	void ClearRenderTargetView(Float4 color = Float4(0.0f, 0.125f, 0.3f, 1.0f));
	void ClearDepthStencilView();

	void Present();

	ID3D11Device* GetDevice() { return mDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext; }
	IDXGISwapChain* GetSwapChain() { return swapChain; }
	ID3D11ShaderResourceView*& GetDepthSRV() { return mDSVsrv; }
	ID3D11ShaderResourceView*& GetBackBufferSRV() { return mRTVsrv; }


private:

	Device();
	~Device();

private:
	friend class Singleton;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	IDXGISwapChain* swapChain;
	vector<ID3D11RenderTargetView*> mRenderTargets;

	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11ShaderResourceView* mRTVsrv;
	ID3D11Texture2D* mRTVtexture;

	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* mDSVtexture;
	ID3D11ShaderResourceView* mDSVsrv;
	bool mbIsStencil;



};