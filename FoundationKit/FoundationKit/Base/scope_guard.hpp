template<typename F>
class scope_guard {
public:
	scope_guard(const F& func) : func_(func) {}
	~scope_guard() { func_(); }

protected:
	F func_;
};