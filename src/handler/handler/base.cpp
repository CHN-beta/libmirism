# include <mirism/detail_/framework/handler/base.hpp>
# include <magic_enum.hpp>


namespace mirism::handler
{
	Base::Base(std::shared_ptr<client::Base> client) : Client_(std::move(client))
	{
		for (auto& timing : magic_enum::enum_values<PatchTiming>())
			PatchList_.push_back({timing, {}});
		PatchList_[PatchTiming::AtClientPatch].
	}
}
