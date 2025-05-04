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
    for target in scratch_json["targets"]:
        for variable_id, variable in target["variables"].items():
            result.append(
                Variable(
                    target["name"],
                    extract_variable_name(variable_id, variable),
                    variable[1],
                )
            )
    return result


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

            c_template = env.get_template("scratch-transpiler-main-template.c")
            c_file.write(
                c_template.render(
                    main_header_file=header_file_path,
                    variables=variables,
                )
            )


def main():
    args = parse_arguments()
    scratch_json = read_scratch_program(args.input)
    compile_scratch_program(scratch_json, args.output)


if __name__ == "__main__":
    main()
