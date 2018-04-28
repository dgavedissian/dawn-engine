/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */

namespace dw {
template <typename Component, typename PropertyType>
SharedPtr<RepPropertyBinding> RepProperty::bind(
    RepProperty::PropertyMemberPtr<Component, PropertyType> member_ptr) {
    return makeShared<RepProperty::RepPropertyBinding_Member<Component, PropertyType>>(member_ptr);
}

template <typename Component, typename PropertyType>
SharedPtr<RepPropertyBinding> RepProperty::bind(
    RepProperty::PropertyReferenceFunc<Component, PropertyType> reference_func) {
    return makeShared<RepProperty::RepPropertyBinding_ReferenceFunction<Component, PropertyType>>(
        reference_func);
}

template <typename Component, typename PropertyType>
SharedPtr<RepPropertyBinding> RepProperty::bind(
    RepProperty::PropertyGetterFunc<Component, PropertyType> getter_func,
    RepProperty::PropertySetterFunc<Component, PropertyType> setter_func) {
    return makeShared<RepProperty::RepPropertyBinding_Accessors<Component, PropertyType>>(
        getter_func, setter_func);
}

template <typename Component>
RepProperty::RepPropertyBindingInComponent<Component>::RepPropertyBindingInComponent()
    : component_(nullptr) {
}

template <typename Component>
void RepProperty::RepPropertyBindingInComponent<Component>::onAddToEntity(Entity& entity) {
    component_ = entity.component<Component>();
    assert(component_ != nullptr);
}

template <typename Component, typename PropertyType>
RepProperty::RepPropertyBinding_Member<Component, PropertyType>::RepPropertyBinding_Member(
    RepProperty::PropertyMemberPtr<Component, PropertyType> member_ptr)
    : member_ptr_(member_ptr) {
}

template <typename Component, typename PropertyType>
void RepProperty::RepPropertyBinding_Member<Component, PropertyType>::serialise(OutputStream& out) {
    stream::write<PropertyType>(out, this->component_->*member_ptr_);
}

template <typename Component, typename PropertyType>
void RepProperty::RepPropertyBinding_Member<Component, PropertyType>::deserialise(InputStream& in) {
    this->component_->*member_ptr_ = stream::read<PropertyType>(in);
}

template <typename Component, typename PropertyType>
RepProperty::RepPropertyBinding_ReferenceFunction<Component, PropertyType>::
    RepPropertyBinding_ReferenceFunction(
        RepProperty::PropertyReferenceFunc<Component, PropertyType> reference_func)
    : reference_func_(reference_func) {
}

template <typename Component, typename PropertyType>
void RepProperty::RepPropertyBinding_ReferenceFunction<Component, PropertyType>::serialise(
    OutputStream& out) {
    stream::write<PropertyType>(out, (this->component_->*reference_func_)());
}

template <typename Component, typename PropertyType>
void RepProperty::RepPropertyBinding_ReferenceFunction<Component, PropertyType>::deserialise(
    InputStream& in) {
    (this->component_->*reference_func_)() = stream::read<PropertyType>(in);
}

template <typename Component, typename PropertyType>
RepProperty::RepPropertyBinding_Accessors<Component, PropertyType>::RepPropertyBinding_Accessors(
    RepProperty::PropertyGetterFunc<Component, PropertyType> getter,
    RepProperty::PropertySetterFunc<Component, PropertyType> setter)
    : getter_func_(getter), setter_func_(setter) {
}

template <typename Component, typename PropertyType>
void RepProperty::RepPropertyBinding_Accessors<Component, PropertyType>::serialise(
    OutputStream& out) {
    stream::write<PropertyType>(out, (this->component_->*getter_func_)());
}

template <typename Component, typename PropertyType>
void RepProperty::RepPropertyBinding_Accessors<Component, PropertyType>::deserialise(
    InputStream& in) {
    (this->component_->*setter_func_)(stream::read<PropertyType>(in));
}
}  // namespace dw