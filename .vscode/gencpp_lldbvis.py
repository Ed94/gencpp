import lldb

class String_SyntheticChildrenProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 3

    def get_child_index(self, name):
        if name == "Data":
            return 0
        if name == "Length":
            return 1
        if name == "Capacity":
            return 2

        return None

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("Data")

        data = self.valobj.GetChildMemberWithName("Data")
        header_ptr = data.GetValueAsUnsigned() - 16
        target = self.valobj.GetTarget()
        header_type = target.FindFirstType("gen::String::Header")
        header = self.valobj.CreateValueFromAddress("Header", header_ptr, header_type)

        if index == 1:
            return header.GetChildMemberWithName("Length")

        if index == 2:
            return header.GetChildMemberWithName("Capacity")

        return None

    def update(self):
        pass

def list_synthetic_providers(debugger):
    print("Listing synthetic providers (start)")

    num_categories = debugger.GetNumCategories()
    print("Debugger language categories count:", num_categories)

    cpp_category = None
    for i in range(num_categories):
        print("WERE HERE")
        print(debugger)
        cat = debugger.GetCategoryAtIndex(i)
        print("Category name: {}, language: {}".format(cat.GetName(), cat.GetLanguage()))
        if cat.GetLanguage() == lldb.eLanguageTypeC_plus_plus:
            cpp_category = cat
            break

    if not cpp_category:
        print("Could not get C++ category")
        return

    synthetic_providers = cpp_category.GetSyntheticChildren()
    if not synthetic_providers:
        print("Could not get synthetic children")
        return

    num_providers = synthetic_providers.GetSize()
    print("Number of synthetic providers:", num_providers)

    for i in range(num_providers):
        provider = synthetic_providers.GetSyntheticChildAtIndex(i)
        print("Provider regex: {}, class name: {}".format(provider.GetRegex(), provider.GetDescription()))

    print("Listing synthetic providers (finish)")




def __lldb_init_module(debugger, internal_dict):
    print("Importing the String visualization")
    debugger.HandleCommand("type synthetic add -x '^gen::String$' -l gencpp_libvis.String_SyntheticChildrenProvider")
    print("Before list_synthetic_providers")
    list_synthetic_providers(debugger)

lldb.debugger = None
