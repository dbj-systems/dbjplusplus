

	// CLASS TEMPLATE array
template<class _Ty,
	size_t _Size>
	class array
	{	// fixed size array of values
public:
	using value_type = _Ty;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using pointer = _Ty *;
	using const_pointer = const _Ty *;
	using reference = _Ty&;
	using const_reference = const _Ty&;

	using iterator = _Array_iterator<_Ty, _Size>;
	using const_iterator = _Array_const_iterator<_Ty, _Size>;

	using reverse_iterator = _STD reverse_iterator<iterator>;
	using const_reverse_iterator = _STD reverse_iterator<const_iterator>;

#if _HAS_TR1_NAMESPACE
	_DEPRECATE_TR1_NAMESPACE void assign(const _Ty& _Value)
		{	// assign value to all elements
		_STD fill_n(_Elems, _Size, _Value);
		}
#endif /* _HAS_TR1_NAMESPACE */

	void fill(const _Ty& _Value)
		{	// assign value to all elements
		_STD fill_n(_Elems, _Size, _Value);
		}

	void swap(array& _Other)
		_NOEXCEPT_COND(_Is_nothrow_swappable<_Ty>::value)
		{	// swap contents with _Other
		_Swap_ranges_unchecked(_Elems, _Elems + _Size, _Other._Elems);
		}

	_NODISCARD _CONSTEXPR17 iterator begin() noexcept
		{	// return iterator for beginning of mutable sequence
		return (iterator(_Elems, 0));
		}

	_NODISCARD _CONSTEXPR17 const_iterator begin() const noexcept
		{	// return iterator for beginning of nonmutable sequence
		return (const_iterator(_Elems, 0));
		}

	_NODISCARD _CONSTEXPR17 iterator end() noexcept
		{	// return iterator for end of mutable sequence
		return (iterator(_Elems, _Size));
		}

	_NODISCARD _CONSTEXPR17 const_iterator end() const noexcept
		{	// return iterator for beginning of nonmutable sequence
		return (const_iterator(_Elems, _Size));
		}

	_NODISCARD _CONSTEXPR17 reverse_iterator rbegin() noexcept
		{	// return iterator for beginning of reversed mutable sequence
		return (reverse_iterator(end()));
		}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator rbegin() const noexcept
		{	// return iterator for beginning of reversed nonmutable sequence
		return (const_reverse_iterator(end()));
		}

	_NODISCARD _CONSTEXPR17 reverse_iterator rend() noexcept
		{	// return iterator for end of reversed mutable sequence
		return (reverse_iterator(begin()));
		}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator rend() const noexcept
		{	// return iterator for end of reversed nonmutable sequence
		return (const_reverse_iterator(begin()));
		}

	_NODISCARD _CONSTEXPR17 const_iterator cbegin() const noexcept
		{	// return iterator for beginning of nonmutable sequence
		return (begin());
		}

	_NODISCARD _CONSTEXPR17 const_iterator cend() const noexcept
		{	// return iterator for end of nonmutable sequence
		return (end());
		}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator crbegin() const noexcept
		{	// return iterator for beginning of reversed nonmutable sequence
		return (rbegin());
		}

	_NODISCARD _CONSTEXPR17 const_reverse_iterator crend() const noexcept
		{	// return iterator for end of reversed nonmutable sequence
		return (rend());
		}

	_CONSTEXPR17 _Ty * _Unchecked_begin() noexcept
		{	// return plain pointer to beginning of mutable sequence
		return (_Elems);
		}

	_CONSTEXPR17 const _Ty * _Unchecked_begin() const noexcept
		{	// return plain pointer to beginning of nonmutable sequence
		return (_Elems);
		}

	_CONSTEXPR17 _Ty * _Unchecked_end() noexcept
		{	// return plain pointer to end of mutable sequence
		return (_Elems + _Size);
		}

	_CONSTEXPR17 const _Ty * _Unchecked_end() const noexcept
		{	// return plain pointer to end of nonmutable sequence
		return (_Elems + _Size);
		}

	_NODISCARD constexpr size_type size() const noexcept
		{	// return length of sequence
		return (_Size);
		}

	_NODISCARD constexpr size_type max_size() const noexcept
		{	// return maximum possible length of sequence
		return (_Size);
		}

	_NODISCARD constexpr bool empty() const noexcept
		{	// test if sequence is empty
		return (false);
		}

	_NODISCARD _CONSTEXPR17 reference at(size_type _Pos)
		{	// subscript mutable sequence with checking
		if (_Size <= _Pos)
			{
			_Xran();
			}

		return (_Elems[_Pos]);
		}

	_NODISCARD constexpr const_reference at(size_type _Pos) const
		{	// subscript nonmutable sequence with checking
		if (_Size <= _Pos)
			{
			_Xran();
			}

		return (_Elems[_Pos]);
		}

	_NODISCARD _CONSTEXPR17 reference operator[](_In_range_(0, _Size - 1) size_type _Pos) noexcept // strengthened
		{	// subscript mutable sequence
 #if _ITERATOR_DEBUG_LEVEL != 0
		_STL_VERIFY(_Pos < _Size, "array subscript out of range");
 #endif /* _ITERATOR_DEBUG_LEVEL != 0 */

		return (_Elems[_Pos]);
		}

	_NODISCARD constexpr const_reference operator[](_In_range_(0, _Size - 1) size_type _Pos) const
		noexcept // strengthened
		{	// subscript nonmutable sequence
 #if _ITERATOR_DEBUG_LEVEL != 0
		_STL_VERIFY(_Pos < _Size, "array subscript out of range");
 #endif /* _ITERATOR_DEBUG_LEVEL != 0 */

		return (_Elems[_Pos]);
		}

	_NODISCARD _CONSTEXPR17 reference front() noexcept // strengthened
		{	// return first element of mutable sequence
		return (_Elems[0]);
		}

	_NODISCARD constexpr const_reference front() const noexcept // strengthened
		{	// return first element of nonmutable sequence
		return (_Elems[0]);
		}

	_NODISCARD _CONSTEXPR17 reference back() noexcept // strengthened
		{	// return last element of mutable sequence
		return (_Elems[_Size - 1]);
		}

	_NODISCARD constexpr const_reference back() const noexcept // strengthened
		{	// return last element of nonmutable sequence
		return (_Elems[_Size - 1]);
		}

	_NODISCARD _CONSTEXPR17 _Ty * data() noexcept
		{	// return pointer to mutable data array
		return (_Elems);
		}

	_NODISCARD _CONSTEXPR17 const _Ty * data() const noexcept
		{	// return pointer to nonmutable data array
		return (_Elems);
		}

	[[noreturn]] void _Xran() const
		{	// report an out_of_range error
		_Xout_of_range("invalid array<T, N> subscript");
		}

	_Ty _Elems[_Size];
	};
