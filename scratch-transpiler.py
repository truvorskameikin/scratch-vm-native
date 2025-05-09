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


class Variable:
    def __init__(self, sprite_name: str, name: str, value: str):
        self.sprite_name = sprite_name
        self.name = name
        try:
            self.value = float(value)
            self.is_string = False
        except:
            self.value = value
            self.is_string = True


def extract_variable_name(scratch_variable_id, scratch_variable):
    return scratch_variable[0].replace(" ", "_")


def extract_variables(scratch_json) -> list[Variable]:
    result = []
    for scratch_target in scratch_json["targets"]:
        for variable_id, variable in scratch_target["variables"].items():
            result.append(
                Variable(
                    scratch_target["name"],
                    extract_variable_name(variable_id, variable),
                    variable[1],
                )
            )
    return result


class SpriteName(str):
    pass


class Sprite:
    def __init__(self, sprite_name: SpriteName, is_stage: bool):
        self.sprite_name = sprite_name
        self.is_stage = is_stage


def extract_sprite_name(scratch_target) -> SpriteName:
    return scratch_target["name"]


def extract_sprites(scratch_json) -> dict[SpriteName, Sprite]:
    result: dict[SpriteName, Sprite] = {}
    for scratch_target in scratch_json["targets"]:
        sprite_name = extract_sprite_name(scratch_target)
        result[sprite_name] = Sprite(sprite_name, scratch_target["isStage"])


class BlockName(str):
    pass


class BlockId(str):
    pass


class Block:
    def __init__(self, op_code: str, is_top_level: bool):
        self.op_code = op_code
        self.is_top_level = is_top_level
        self.scratch_inplace_blocks = []
        self.scratch_inplace_blocks_op_codes = []
        self.block_name = ""
        self.next_block_name = ""
        self.substack_block_name = ""

    def set_inplace_blocks(self, scratch_inplace_blocks):
        self.scratch_inplace_blocks = scratch_inplace_blocks
        for b in self.scratch_inplace_blocks:
            self.scratch_inplace_blocks_op_codes.append(b["opcode"])

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


def extract_block_name(scratch_target, scratch_block, block_number: int) -> BlockName:
    return (
        extract_sprite_name(scratch_target)
        + "_"
        + scratch_block["opcode"]
        + "_"
        + str(block_number)
    )


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


def extract_blocks_r(scratch_target, scratch_block, all_blocks):
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
                block = Block(kOpcodeRunInplace, scratch_cur_block["topLevel"])
                block.set_inplace_blocks(inplace_blocks)
                block.block_name = f"{extract_sprite_name(scratch_target)}_Inplace{str(len(all_blocks))}"
                all_blocks.append(block)

                result = extract_blocks_r(scratch_target, scratch_cur_block, all_blocks)
                if result is not None:
                    block.next_block_name = result.block_name

                return block
            else:
                block = Block(
                    to_known_op_codes(scratch_cur_block["opcode"]),
                    scratch_cur_block["topLevel"],
                )
                op_code = scratch_cur_block["opcode"]
                block.block_name = f"{extract_sprite_name(scratch_target)}_{op_code}{str(len(all_blocks))}"
                all_blocks.append(block)

                if has_substack(scratch_cur_block):
                    substack_id = scratch_cur_block["inputs"]["SUBSTACK"][1]
                    substack_result = extract_blocks_r(
                        scratch_target,
                        scratch_target["blocks"][substack_id],
                        all_blocks,
                    )

                    if substack_result:
                        block.substack_block_name = substack_result.block_name

                result = extract_blocks_r(
                    scratch_target, scratch_next_block, all_blocks
                )
                if result is not None:
                    block.next_block_name = result.block_name

                return block

    if len(inplace_blocks) > 0:
        block = Block(kOpcodeRunInplace, scratch_cur_block["topLevel"])
        block.set_inplace_blocks(inplace_blocks)
        block.block_name = (
            f"{extract_sprite_name(scratch_target)}_Inplace{str(len(all_blocks))}"
        )
        all_blocks.append(block)

        return block

    return block


def extract_blocks(scratch_json):
    all_blocks = []
    for scratch_target in scratch_json["targets"]:
        top_level_block_ids = []
        for scratch_block_id, scratch_block in scratch_target["blocks"].items():
            if scratch_block["topLevel"]:
                top_level_block_ids.append(scratch_block_id)

        for scratch_top_level_block_id in top_level_block_ids:
            extract_blocks_r(
                scratch_target,
                scratch_target["blocks"][scratch_top_level_block_id],
                all_blocks,
            )
    return all_blocks


def compile_scratch_program(scratch_json, output_stem: str):
    header_file_path = f"{output_stem}.h"
    c_file_path = f"{output_stem}.c"

    with open(header_file_path, "w") as header_file:
        with open(c_file_path, "w") as c_file:
            env = Environment(
                loader=PackageLoader("scratch-transpiler"),
                autoescape=select_autoescape(),
            )

            header_template = env.get_template("scratch-transpiler-main-template.h")
            header_file.write(header_template.render())

            variables = extract_variables(scratch_json)
            sprites = extract_sprites(scratch_json)
            blocks = extract_blocks(scratch_json)

            c_template = env.get_template("scratch-transpiler-main-template.c")
            c_file.write(
                c_template.render(
                    main_header_file=header_file_path,
                    variables=variables,
                    blocks=blocks,
                )
            )


def main():
    args = parse_arguments()
    scratch_json = read_scratch_program(args.input)
    compile_scratch_program(scratch_json, args.output)


if __name__ == "__main__":
    main()
