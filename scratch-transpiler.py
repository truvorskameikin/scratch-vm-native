import argparse
import zipfile
import json
from jinja2 import Environment, PackageLoader, select_autoescape


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="Scratch Transpiler",
        description="Compiles Scratch programs outputing C code",
    )

    parser.add_argument("-i", "--input", help="Input Scratch program (*.sb3).")
    parser.add_argument("-o", "--output", help="Output files stem.")

    return parser.parse_args()


def read_scratch_program(file_path: str):
    with zipfile.ZipFile(file_path) as file:
        with file.open("project.json") as project_file:
            return json.loads(project_file.read())


def extract_sprite_name(scratch_target) -> str:
    return scratch_target["name"].replace(" ", "_")


class Target:
    def __init__(self, sprite_name):
        self.sprite_name = sprite_name
        self.c_struct_name = f"{self.sprite_name}_t"
        self.variable_name = self.sprite_name
        self.clone_c_struct_name = f"{self.sprite_name}_Clone_t"
        self.per_level_runtimes = []

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return f"Target({self.sprite_name}, {self.c_struct_name})"


class Variable:
    def __init__(
        self, scratch_target: str, variable_name: str, value: str, scratch_variable_name
    ):
        self.scratch_target = scratch_target
        self.variable_name = variable_name
        self.scratch_variable_name = scratch_variable_name
        self.scratch_target_name = self.scratch_target["name"]
        try:
            self.value = float(value)
            self.is_string = False
        except:
            self.value = value
            self.is_string = True

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return f"Variable({self.variable_name}: {self.value})"


def extract_variable(scratch_json, scratch_target, scratch_variable_id) -> Variable:
    if scratch_variable_id in scratch_target["variables"]:
        variable_value = scratch_target["variables"][scratch_variable_id][1]
        scratch_variable_name = scratch_target["variables"][scratch_variable_id][0]
        return Variable(
            scratch_target,
            extract_sprite_name(scratch_target)
            + "_"
            + scratch_variable_name.replace(" ", "_"),
            variable_value,
            scratch_variable_name,
        )

    scratch_stage_target = None
    for scratch_target in scratch_json["targets"]:
        if scratch_target["isStage"]:
            scratch_stage_target = scratch_target
            break

    return extract_variable(scratch_json, scratch_stage_target, scratch_variable_id)


def access_variable(
    scratch_json, scratch_target, scratch_variable_id, all_variables_and_cache
):
    variable = extract_variable(scratch_json, scratch_target, scratch_variable_id)
    if variable.variable_name in all_variables_and_cache["cache"]:
        return

    all_variables_and_cache["all_variables"].append(variable)
    all_variables_and_cache["cache"][variable.variable_name] = scratch_variable_id


class Helper:
    def __init__(self, op_code, function_name):
        self.op_code = op_code
        self.function_name = function_name
        self.arguments = []

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return f"Helper({self.op_code}, {self.function_name})"


def add_new_helper(helper, helpers, helpers_count_obj):
    count = helpers_count_obj["count"]
    helpers.append(helper)
    helpers_count_obj["count"] = helpers_count_obj["count"] + 1


def extract_inline_helpers_inputs_r(
    scratch_json, scratch_target, input_obj, helpers, count_obj
):
    value_input_type = input_obj[0]
    value = input_obj[1]

    # Shadow
    if value_input_type == 1:
        # value is array [type, value]. 4, 5, 6, 7 - integer values; 8 - angle; 9 - color; 10 - string (can be converted to int)

        count = count_obj["count"]

        opcode = "read_value_string"
        if value[0] in (4, 5, 6, 7):
            opcode = "read_value_number"
        if value[0] == 10:
            try:
                float(value[1])
                opcode = "read_value_number"
            except:
                opcode = "read_value_string"

        function_name = f"{extract_sprite_name(scratch_target)}_{opcode}_helper_{count}"
        helper = Helper(opcode, function_name)

        helper.arguments = [value[1]]

        add_new_helper(helper, helpers, count_obj)

    if value_input_type == 3:
        if isinstance(value, list):
            # Variable
            if value[0] == 12:
                count = count_obj["count"]
                opcode = "read_variable"
                function_name = (
                    f"{extract_sprite_name(scratch_target)}_{opcode}_helper_{count}"
                )

                helper = Helper(opcode, function_name)
                variable = extract_variable(scratch_json, scratch_target, value[2])
                helper.arguments = [variable.variable_name]

                add_new_helper(helper, helpers, count_obj)
        else:
            # value is input ID
            next_block = scratch_target["blocks"][value]
            extract_inline_helpers_r(
                scratch_json, scratch_target, next_block, helpers, count_obj
            )


def extract_inline_helpers_r(
    scratch_json, scratch_target, scratch_block, helpers, count_obj
):
    opcode = scratch_block["opcode"]
    if opcode == "data_setvariableto":
        extract_inline_helpers_inputs_r(
            scratch_json,
            scratch_target,
            scratch_block["inputs"]["VALUE"],
            helpers,
            count_obj,
        )

        variable_id = scratch_block["fields"]["VARIABLE"][1]
        variable = extract_variable(scratch_json, scratch_target, variable_id)

        count = count_obj["count"]
        function_name = f"{extract_sprite_name(scratch_target)}_set_variable_{count}"
        helper = Helper("set_variable", function_name)
        if len(helpers) > 0:
            value_helper = helpers[-1]
            helper.arguments = [value_helper.function_name, variable.variable_name]
        add_new_helper(helper, helpers, count_obj)

    if opcode == "operator_mathop":
        extract_inline_helpers_inputs_r(
            scratch_json,
            scratch_target,
            scratch_block["inputs"]["NUM"],
            helpers,
            count_obj,
        )

        num_helper = helpers[-1]

        operator = scratch_block["fields"]["OPERATOR"][0]

        count = count_obj["count"]
        function_name = f"{extract_sprite_name(scratch_target)}_{operator}_{count}"
        helper = Helper(operator, function_name)
        helper.arguments = [num_helper.function_name]
        add_new_helper(helper, helpers, count_obj)

    if (
        opcode == "operator_divide"
        or opcode == "operator_add"
        or opcode == "operator_multiply"
    ):
        extract_inline_helpers_inputs_r(
            scratch_json,
            scratch_target,
            scratch_block["inputs"]["NUM1"],
            helpers,
            count_obj,
        )
        num1_helper = helpers[-1]

        extract_inline_helpers_inputs_r(
            scratch_json,
            scratch_target,
            scratch_block["inputs"]["NUM2"],
            helpers,
            count_obj,
        )
        num2_helper = helpers[-1]

        count = count_obj["count"]
        function_name = f"{extract_sprite_name(scratch_target)}_{opcode}_{count}"
        helper = Helper(opcode, function_name)
        helper.arguments = [num1_helper.function_name, num2_helper.function_name]
        add_new_helper(helper, helpers, count_obj)

    if opcode == "operator_join":
        extract_inline_helpers_inputs_r(
            scratch_json,
            scratch_target,
            scratch_block["inputs"]["STRING1"],
            helpers,
            count_obj,
        )
        num1_helper = helpers[-1]

        extract_inline_helpers_inputs_r(
            scratch_json,
            scratch_target,
            scratch_block["inputs"]["STRING2"],
            helpers,
            count_obj,
        )
        num2_helper = helpers[-1]

        count = count_obj["count"]
        function_name = f"{extract_sprite_name(scratch_target)}_{opcode}_{count}"
        helper = Helper(opcode, function_name)
        helper.arguments = [num1_helper.function_name, num2_helper.function_name]
        add_new_helper(helper, helpers, count_obj)


class Block:
    def __init__(self, target: Target, op_code: str, is_top_level: bool, level):
        self.target = target
        self.op_code = op_code
        self.is_top_level = is_top_level
        self.level = level
        self.max_level = 0
        self.scratch_inplace_blocks = []
        self.scratch_inplace_blocks_op_codes = []
        self.scratch_inplace_blocks_helpers = []
        self.scratch_functions = []
        self.block_name = ""
        self.next_block_name = ""
        self.substack_block_name = ""

    def set_inplace_blocks(
        self,
        scratch_json,
        scratch_target,
        helpers_count_obj,
        all_variables_and_cache,
        scratch_inplace_blocks,
    ):
        self.scratch_inplace_blocks = scratch_inplace_blocks
        for b in self.scratch_inplace_blocks:
            self.scratch_inplace_blocks_op_codes.append(b["opcode"])

            has_helper = False
            if b["opcode"] == "data_setvariableto":
                variable_id = b["fields"]["VARIABLE"][1]

                access_variable(
                    scratch_json, scratch_target, variable_id, all_variables_and_cache
                )

            helpers = []
            extract_inline_helpers_r(
                scratch_json,
                scratch_target,
                b,
                helpers,
                helpers_count_obj,
            )
            self.scratch_inplace_blocks_helpers.append(helpers)
            self.scratch_functions.append(helpers[-1].function_name)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        inplace_blocks = ""
        if self.scratch_inplace_blocks_op_codes:
            inplace_blocks = f" Inplace: {self.scratch_inplace_blocks_op_codes}"

        substack = ""
        if self.substack_block_name:
            substack = f" Substack: {self.substack_block_name}"
        return f"Block({self.block_name}{inplace_blocks}{substack} -> {self.next_block_name})"


def can_run_inplace(scratch_block) -> bool:
    if scratch_block["opcode"] == "data_setvariableto":
        return True
    if scratch_block["opcode"] == "motion_setx":
        return True

    return False


def has_substack(scratch_block) -> bool:
    if scratch_block["opcode"] == "control_forever":
        return True
    if scratch_block["opcode"] == "control_if":
        return True

    return False


kOpcodeRunInplace = "kScratchInPlace"


def to_known_op_codes(scratch_op_code):
    if scratch_op_code == "event_whenflagclicked":
        return "kScratchWhenFlagClicked"
    if scratch_op_code == "control_forever":
        return "kScratchControlForever"
    if scratch_op_code == "control_if":
        return "kScratchControlIf"
    if scratch_op_code == "control_wait":
        return "kScratchControlWait"


def extract_targets_blocks_and_variables_r(
    scratch_json,
    scratch_target,
    helpers_count_obj,
    scratch_block,
    level,
    all_targets,
    all_blocks,
    all_variables_and_cache,
):
    inplace_blocks = []

    block = None

    scratch_next_block = scratch_block
    while scratch_next_block:
        scratch_cur_block = scratch_next_block
        if not scratch_cur_block["next"]:
            scratch_next_block = None
        else:
            scratch_next_block = scratch_target["blocks"][scratch_cur_block["next"]]
        if can_run_inplace(scratch_cur_block):
            inplace_blocks.append(scratch_cur_block)
        else:
            if len(inplace_blocks) > 0:
                block = Block(
                    all_targets[-1],
                    kOpcodeRunInplace,
                    scratch_cur_block["topLevel"],
                    level,
                )
                block.max_level = level
                block.set_inplace_blocks(
                    scratch_json,
                    scratch_target,
                    helpers_count_obj,
                    all_variables_and_cache,
                    inplace_blocks,
                )
                block.block_name = f"{extract_sprite_name(scratch_target)}_inplace{str(len(all_blocks))}"
                all_blocks.append(block)

                result = extract_targets_blocks_and_variables_r(
                    scratch_json,
                    scratch_target,
                    helpers_count_obj,
                    scratch_cur_block,
                    level,
                    all_targets,
                    all_blocks,
                    all_variables_and_cache,
                )
                if result is not None:
                    block.next_block_name = result.block_name

                    max_level = max(block.max_level, result.max_level)
                    block.max_level = max_level
                    result.max_level = max_level

                return block
            else:
                block = Block(
                    all_targets[-1],
                    to_known_op_codes(scratch_cur_block["opcode"]),
                    scratch_cur_block["topLevel"],
                    level,
                )
                block.max_level = level
                op_code = scratch_cur_block["opcode"]
                block.block_name = f"{extract_sprite_name(scratch_target)}_{op_code}{str(len(all_blocks))}"
                all_blocks.append(block)

                if has_substack(scratch_cur_block):
                    substack_id = scratch_cur_block["inputs"]["SUBSTACK"][1]
                    substack_result = extract_targets_blocks_and_variables_r(
                        scratch_json,
                        scratch_target,
                        helpers_count_obj,
                        scratch_target["blocks"][substack_id],
                        level + 1,
                        all_targets,
                        all_blocks,
                        all_variables_and_cache,
                    )

                    if substack_result:
                        block.substack_block_name = substack_result.block_name

                        max_level = max(block.max_level, substack_result.max_level)
                        block.max_level = max_level
                        substack_result.max_level = max_level

                result = extract_targets_blocks_and_variables_r(
                    scratch_json,
                    scratch_target,
                    helpers_count_obj,
                    scratch_next_block,
                    level,
                    all_targets,
                    all_blocks,
                    all_variables_and_cache,
                )
                if result is not None:
                    block.next_block_name = result.block_name

                    max_level = max(block.max_level, result.max_level)
                    block.max_level = max_level
                    result.max_level = max_level

                return block

    if len(inplace_blocks) > 0:
        block = Block(
            all_targets[-1], kOpcodeRunInplace, scratch_cur_block["topLevel"], level
        )
        block.max_level = level
        block.set_inplace_blocks(
            scratch_json,
            scratch_target,
            helpers_count_obj,
            all_variables_and_cache,
            inplace_blocks,
        )
        block.block_name = (
            f"{extract_sprite_name(scratch_target)}_inplace{str(len(all_blocks))}"
        )
        all_blocks.append(block)

        return block

    return block


def extract_targets_blocks_and_variables(scratch_json):
    all_targets = []
    all_blocks = []
    all_variables_and_cache = {"all_variables": [], "cache": {}}
    for scratch_target in scratch_json["targets"]:
        sprite_name = extract_sprite_name(scratch_target)
        target = Target(sprite_name)
        all_targets.append(target)

        top_level_block_ids = []
        for scratch_block_id, scratch_block in scratch_target["blocks"].items():
            if scratch_block["topLevel"]:
                top_level_block_ids.append(scratch_block_id)

        helpers_count_obj = {"count": 0}
        for scratch_top_level_block_id in top_level_block_ids:
            extract_targets_blocks_and_variables_r(
                scratch_json,
                scratch_target,
                helpers_count_obj,
                scratch_target["blocks"][scratch_top_level_block_id],
                0,
                all_targets,
                all_blocks,
                all_variables_and_cache,
            )

    return all_targets, all_blocks, all_variables_and_cache["all_variables"]


def compile_scratch_program(scratch_json, output_stem: str):
    header_file_path = f"{output_stem}.h"
    c_file_path = f"{output_stem}.c"

    with open(header_file_path, "w") as header_file:
        with open(c_file_path, "w") as c_file:
            env = Environment(
                loader=PackageLoader("scratch-transpiler"),
                autoescape=select_autoescape(),
                trim_blocks=True,
                lstrip_blocks=True,
            )

            header_template = env.get_template("scratch-transpiler-main-template.h")
            header_file.write(header_template.render())

            targets, blocks, variables = extract_targets_blocks_and_variables(
                scratch_json
            )

            when_flag_clicked_blocks = [
                b for b in blocks if b.op_code == "kScratchWhenFlagClicked"
            ]

            c_template = env.get_template("scratch-transpiler-main-template.c")
            c_file.write(
                c_template.render(
                    main_header_file=header_file_path,
                    targets=targets,
                    blocks=blocks,
                    variables=variables,
                    when_flag_clicked_blocks=when_flag_clicked_blocks,
                )
            )


def main():
    args = parse_arguments()
    scratch_json = read_scratch_program(args.input)
    compile_scratch_program(scratch_json, args.output)


if __name__ == "__main__":
    main()
