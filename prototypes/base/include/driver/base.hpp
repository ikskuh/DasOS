#pragma once

namespace driver
{
	class Driver
	{
	protected:
		Driver() = default;
	public:
		virtual void install() = 0;
	};
}