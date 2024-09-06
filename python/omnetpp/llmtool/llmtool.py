import sys
import logging
import os
import re
import llm
import argparse
import tiktoken
import time

# logging.basicConfig(level=logging.INFO)
_logger = logging.getLogger(__name__)
_logger.setLevel(logging.INFO)

def read_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        return file.read()

def write_file(file_path, content):
    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(content)

def read_resource_file(filename):
    file_path = os.path.join(os.path.dirname(__file__), filename)
    return read_file(file_path)

def dict_except(dict, keys):
    return {k: v for k, v in dict.items() if k not in keys}

def replace_extension(file_path, new_ext):
    return os.path.splitext(file_path)[0] + new_ext

def discard_trailing_whitespace(content):
    return re.sub(r'[ \t]+$', '', content, flags=re.MULTILINE)

def collect_matching_file_paths(directory, name_regex=None, content_pattern=None):
    matching_file_paths = []
    content_regex = re.compile(content_pattern) if content_pattern else None

    for root, _, files in os.walk(directory):
        for file in files:
            if name_regex is None or name_regex.match(file):
                file_path = os.path.join(root, file)
                if not content_regex:
                    matching_file_paths.append(file_path)
                else:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        try:
                            content = f.read()
                            if content_regex.search(content):
                                matching_file_paths.append(file_path)
                        except Exception as e:
                            print(f"Could not read file {file_path}: {e}", file=sys.stderr)

    return matching_file_paths

def read_and_quote_files(file_list):
    contents = ""
    for file_path in file_list:
        content = read_file(file_path)
        contents += f"{file_path}:\n```\n{content}\n```\n\n"
    return contents

def get_llm_context_window(model):
    size = model.default_max_tokens
    if size:  # it is not always filled in?
        return size
    model_name = model.model_id
    context_window_sizes = {
        "gpt-3.5-turbo": 4096,
        "gpt-3.5-turbo-16k": 16384,
        "gpt-4": 8192,
        "gpt-4-32k": 32768,
        "gpt-4o": 128*1024,
        "gpt-4o-mini": 128*1024,
    }
    if model_name not in context_window_sizes:
        raise Exception(f"Context window size for llm '{model_name}' is not known, please add it to the table")
    return context_window_sizes[model_name]

def check_token_count(prompt, model):
    encoder = tiktoken.encoding_for_model(model.model_id)
    num_tokens = len(encoder.encode(prompt))
    max_tokens = get_llm_context_window(model)
    _logger.info(f"Number of tokens in the prompt: {num_tokens}")
    if max_tokens and num_tokens > max_tokens:
        print(f"WARNING: Prompt of {num_tokens} tokens exceeds the model's context window size of {max_tokens} tokens")

def generate_command_text(task, file_type):
    file_type_commands = {
        "md": "The following Markdown file belongs to the OMNeT++ project.",
        "rst": "The following reStructuredText file  belongs to the OMNeT++ project. Blocks starting with '..' are comments, do not touch them!",
        "tex": "The following LaTeX file belongs to the OMNeT++ project.",
        "ned": "The following is an OMNeT++ NED file. DO NOT CHANGE ANYTHING IN NON-COMMENT LINES.",
        "py":  "The following Python source file belongs to the OMNeT++ project.",
        "c++": "The following is a C++ source file that belongs to the OMNeT++ project.",
    }
    what = "docstrings" if file_type == "py" else "text" if file_type == "c++" else "code"
    task_commands = {
        "proofread": f"You are an editor of technical documentation. You are tireless and diligent. Fix any English mistakes in the {what}. Keep all markup, line breaks and indentation intact as much as possible!",
        "improve": "{% include cpp-improve.txt %}" if file_type == "c++" else f"You are an editor of technical documentation. You are tireless and diligent. Improve the English in the {what}. Keep all other markup and line breaks intact as much as possible.",
        "eliminate-you-addressing": f"You are an editor of technical documentation. You are tireless and diligent. At places where {what} addresses the user as 'you', change it to neutral, e.g., to passive voice or 'one' as subject. Keep all markup and line breaks intact as much as possible.",
        "neddoc": "You are a technical writer. You are tireless and diligent. Write a new neddoc comment for the module in the NED file. NED comments use // marks instead of /*..*/.",
        # "neddoc": """
        # You are a technical writer.  You are tireless and diligent. You are working in the context of OMNeT++ and the INET Framework.
        # Your task is to write a single sentence capturing the most important aspect of the following simple module.
        # Ignore the operational details of the module and focus on the aspects that help the user understand what this module is good for.
        #   """
    }

    if file_type not in file_type_commands:
        raise ValueError(f'Unsupported file type "{file_type}"')
    if task not in task_commands:
        raise ValueError(f'Unsupported task "{task}"')
    if "ned" in task and file_type != "ned":
        raise ValueError(f'Task "{task}" is only supported for the "ned" file type')

    file_type_command = file_type_commands[file_type]

    return file_type_command + " " + task_commands[task]

def get_recommended_model(task, file_type):
    recommended_models = {
        "default": "gpt-4o-mini",
        "proofread": "gpt-4o-mini",
        "improve": "gpt-4o-mini",
        "eliminate-you-addressing": "gpt-4o-mini",
        "neddoc": "gpt-4o-mini"
    }
    key = task if task in recommended_models else "default"
    return recommended_models[key]

def get_default_reply_format(file_type, task):
    #return "patch" if file_type == "c++" else "updated-content" -- not used because replies are much lower quality with "patch"
    return "updated-content"

def find_additional_context_files(file_path, file_type, task):
    context_files = []
    if file_type == "ned":
        # fname_without_ext = os.path.splitext(os.path.basename(file_path))[0]
        # h_fname = fname_without_ext + ".h"
        # cc_fname = fname_without_ext + ".cc"
        # for root, _, files in os.walk(os.path.dirname(file_path) or "."):
        #     if h_fname in files:
        #         context_files.append(os.path.join(root, h_fname))
        #     if cc_fname in files:
        #         context_files.append(os.path.join(root, cc_fname))

        fname_without_ext = os.path.splitext(os.path.basename(file_path))[0]
        grep_command = f"""rg --heading -g '*.cc' -g '*.h' -g '*.ini' -g '*.ned' -g '!{os.path.basename(file_path)}' -C 20 '{fname_without_ext}' $(git rev-parse --show-toplevel) | head -n 3000 > {file_path}.ctx"""
        os.system(grep_command)
        context_files.append(file_path + ".ctx")
    elif file_type == "c++":
        def append_if_file_exists(file_path):
            if os.path.exists(file_path):
                context_files.append(file_path)
        if file_path.endswith(".cc"):
            append_if_file_exists(replace_extension(file_path, ".h"))
        if file_path.endswith(".h"):
            append_if_file_exists(replace_extension(file_path, ".cc"))
    return context_files

def create_prompt(file_path, content, context, task, custom_prompt, file_type, reply_format, region_seq=None, part_seq=None, num_parts=None, save_prompt=False):
    base_prompt = "Update a file.\n" if region_seq is None and part_seq is None and num_parts is None else "Update a portion of a file.\n"
    context_prompt = f"Here is some content which is related to the file that should be updated. Use it as context that provides addition information for the task. \n{context}\n" if context else ""
    command_prompt = custom_prompt if custom_prompt else generate_command_text(task, file_type)

    if reply_format == "patch":
        reply_prompt = read_resource_file("patch-format.txt")
    elif reply_format == "updated-content":
        reply_prompt = "Respond with the updated content verbatim without any additional commentary.\n"
    else:
        raise ValueError(f'Unsupported reply type "{reply_format}"')

    if content is None:
        assert region_seq is None and part_seq is None and num_parts is None
        content = read_file(file_path)

    file_prompt = ""
    if region_seq is not None:
        file_prompt = "The content below is just a portion of a file. Do NOT generate any additional code in an attempt to \"complete\" the file.\n"

    if num_parts is None or num_parts == 1:
        file_prompt += f"Here is the content that should be updated, enclosed in triple backticks:\n\n{file_path}:\n```\n{content}\n```\n"
    else:
        file_prompt += f"The content to be updated is long, so it is split into parts. Here is part {part_seq} of {num_parts} parts total:\n\n{file_path}, part {part_seq} of {num_parts}:\n```\n{content}\n```\n"

    prompt = f"{base_prompt}\n{context_prompt}\n{command_prompt}\n{reply_prompt}\n{file_prompt}"

    def resolve_includes(text):
        def read_file_or_resource_file(file_name):
            return read_resource_file(file_name) if os.path.exists(file_name) else read_resource_file(file_name)
        text = re.sub(r"{%\s*include (.+?)\s*%}", lambda m: resolve_includes(read_file_or_resource_file(m.group(1))), text)
        return text

    # Resolve {% include file_name %} directives, i.e. replace them with the content of file_name.
    # If file_name is not found, assumes it names a built-in resource file and looks for it in this script's folder.
    prompt = resolve_includes(prompt)

    if save_prompt:
        suffix = "" if region_seq is None else f".region{region_seq}"
        suffix += "" if num_parts is None or num_parts == 1 else f".part{part_seq}"
        write_file(file_path + ".prompt" + suffix, prompt)

    return prompt

def invoke_llm(prompt, model, print_stats=False):
    check_token_count(prompt, model)

    def info(txt):
        lines = txt.count("\n") + 1
        chars = len(txt)
        return f"{lines:,} li / {chars:,} ch"

    _logger.debug(f"Sending prompt to LLM: {prompt}")
    if print_stats:
        print(f"     sending prompt: {info(prompt)}", end="", flush=True)
    start_time = time.perf_counter()
    reply = model.prompt(prompt)
    reply_text = reply.text()
    end_time = time.perf_counter()
    if print_stats:
        print(f" --> reply {info(reply_text)} ({end_time - start_time:.2f} sec)")
    _logger.debug(f"Received result from LLM: {reply_text}")
    return reply_text

def split_content(file_content, file_type, max_chars):
    if file_type == "tex":
        parts = split_latex_by_sections(file_content, max_chars)
    elif file_type == "rst":
        parts = split_rst_by_headings(file_content, max_chars)
    elif file_type == "py":
        parts = split_python_by_defs(file_content, max_chars)
    elif file_type == "c++":
        parts = split_cpp_by_defs(file_content, min(15000, max_chars))  # gpt-4o appears to stop generating after outputting ~16k chars of source code
    else:
        parts = split_by_regex(file_content, "\n", max_chars)
    if "".join(parts) != file_content:
        joined_content = "".join(parts)
        raise ValueError(f"Split content does not match original content; {len(file_content)=}, {len(joined_content)=}")
    return parts

def split_latex_by_sections(latex_source, max_chars):
    # Regex to match section headers
    pattern = r'(\\section\{.*?\})'
    return split_by_regex(latex_source, pattern, max_chars)

def split_rst_by_headings(rst_source, max_chars):
    # Regular expression to match RST headings (title and underline)
    pattern = r'(^.*\n(={3,}|-{3,}|`{3,}|:{3,}|\+{3,}|\*{3,}|\#{3,}|\^{3,}|"{3,}|~{3,})$)'
    return split_by_regex(rst_source, pattern, max_chars)

def split_python_by_defs(python_source, max_chars):
    # Regular expression to match Python toplevel functions and classes
    pattern = r'(^(def|class) )'
    return split_by_regex(python_source, pattern, max_chars)

def split_cpp_by_defs(cpp_source, max_chars):
    # Match end of previous block ("}" at start of line)
    pattern = r'^}\s*\n'
    return split_by_regex(cpp_source, pattern, max_chars, at_match_start_positions=False)

def split_by_regex(text, regex_pattern, max_chars, at_match_start_positions=True):
    # Split to chunks
    regex = re.compile(regex_pattern, re.MULTILINE)
    matches = list(regex.finditer(text))
    split_indices = [0] + [match.start() if at_match_start_positions else match.end() for match in matches] + [len(text)]
    chunks = [text[split_indices[i]:split_indices[i+1]] for i in range(0, len(split_indices)-1)]

    # Merge smaller chunks
    parts = []
    current_part = ""
    for chunk in chunks:
        if len(current_part) + len(chunk) > max_chars:
            parts.append(current_part)
            current_part = ""
        current_part += chunk
    parts.append(current_part)

    return parts

def extract(reply_text, original_content):
    content = reply_text
    if content.count("```") >= 2:
        content = re.sub(r"^.*```.*?\n(.*\n)```.*$", r"\1", content, 1, re.DOTALL)

    trailing_whitespace_len = len(original_content) - len(original_content.rstrip())
    original_trailing_whitespace = original_content[-trailing_whitespace_len:]
    content = content.rstrip() + original_trailing_whitespace
    return content

def parse_replace_blocks(blocks_text):
    def join_lines(lines):
        return "\n".join(lines) + "\n"

    search_replace_blocks = []
    in_search_block = False
    in_replace_block = False
    search_lines = []
    replace_lines = []

    for line in blocks_text.splitlines():
        if line.startswith("```"):
            pass
        elif line == "<<<<<<< SEARCH":
            if in_search_block or in_replace_block:
                # Invalid block if already in a block, so we reset
                print("Warning: Invalid replace block (missing close marker?), skipping")
                in_search_block = False
                in_replace_block = False
                search_lines = []
                replace_lines = []
            in_search_block = True
            search_lines = []
        elif line == "=======":
            if in_search_block:
                in_search_block = False
                in_replace_block = True
                replace_lines = []
            else:
                # Invalid block
                print("Warning: Invalid replace block (stray '======' marker), skipping")
                in_search_block = False
                in_replace_block = False
                search_lines = []
                replace_lines = []
        elif line == ">>>>>>> REPLACE":
            if in_replace_block:
                search_replace_blocks.append((join_lines(search_lines), join_lines(replace_lines)))
                in_replace_block = False
            else:
                # Invalid block
                print("Warning: Invalid replace block (unexpected close marker), skipping")
                in_search_block = False
                in_replace_block = False
                search_lines = []
                replace_lines = []
        elif line.startswith(">>>>>>>") or line.startswith("<<<<<<<") or line.startswith("======="):
            # Invalid marker line
            print("Warning: Invalid marker-like line, skipping")
        else:
            if in_search_block:
                search_lines.append(line)
            elif in_replace_block:
                replace_lines.append(line)
            else:
                pass

    return search_replace_blocks

def apply_replace_blocks(content, blocks):
    for search_text, replace_text in blocks:
        if search_text not in content:
            print(f"Warning: Search text `{search_text}` not found")
        content = content.replace(search_text, replace_text, 1)
    return content

def extract_and_replace_cpp_regions_containing(source, regex):
    spans = find_cpp_regions_containing(source, regex)
    modified_source, placeholders_to_content = replace_spans_with_placeholders(source, spans)
    return modified_source, placeholders_to_content

def find_cpp_regions_containing(source, pattern):
    # Find whole function or class definitions that contain the given regex pattern.
    # Assumes properly formatted C++ code. Does not work properly for one-liner functions
    # where everything is on one line (it selects more than necessary).

    def find_region_start_index(source, start_index):
        # Find the nearest unindented line above start_index, which will likely be the
        # class/struct declaration header, or the header of out of-line method definition.
        # Avoid considering preprocessor directives (`#`), the opening brace placed
        # on its own line (`{`), and empty lines (`\n`),
        matches = list(re.finditer(r'^[^#{ \t\f\n]', source[:start_index], re.MULTILINE))
        return matches[-1].start() if matches else 0

    def find_region_end_index(source, start_index):
        # Find the next unindented close brace.
        match = re.search(r'^\}.*?\n', source[start_index:], re.MULTILINE)
        return start_index + match.end() if match else len(source)

    def merge_spans(spans):
        if len(spans) < 2:
            return spans

        # Sort spans by the start position
        sorted_spans = sorted(spans, key=lambda x: x[0])

        # Initialize the list to hold merged spans
        merged_spans = [sorted_spans[0]]

        for current in sorted_spans[1:]:
            last = merged_spans[-1]

            # If the current span overlaps or is adjacent to the last span, merge them
            if current[0] <= last[1]:
                merged_spans[-1] = (last[0], max(last[1], current[1]))
            else:
                merged_spans.append(current)

        return merged_spans

    # Tweak: it is customary to place the private/protected/public at the start of the line,
    # but we don't want them to act as block start lines. The workaround is to tweak such lines
    # by indenting them by two spaces.

    source = source.replace('\r\n', '\n')
    source = source.replace('\nprivate:', '\n  private:')
    source = source.replace('\nprotected:', '\n  protected:')
    source = source.replace('\npublic:', '\n  public:')

    # Find all occurrences of the regex, then extend their spans to the start/end
    # of the function definition or class declaration.
    matches = re.finditer(pattern, source)
    spans = [ (find_region_start_index(source, m.start()), find_region_end_index(source, m.end())) for m in matches]
    spans = merge_spans(spans)
    return spans

def replace_spans_with_placeholders(text, spans):
    # Sort spans by the start position
    spans = sorted(spans, key=lambda x: x[0])

    placeholder_to_content = {}
    offset = 0
    counter = 1

    # Update text
    for start, end in spans:
        actual_start = start + offset
        actual_end = end + offset
        original_content = text[actual_start:actual_end]
        placeholder = f"<@ PLACEHOLDER_{counter} @>\n"
        text = text[:actual_start] + placeholder + text[actual_end:]
        placeholder_to_content[placeholder] = original_content
        offset += len(placeholder) - (end - start)
        counter += 1

    return text, placeholder_to_content

def extract_and_replace_regions_between(text, start_regex, end_regex):
    # example: to match constructor implementations in cc files, try this:
    # --start-at='\b(\w+)::\1' --end-at='^}'

    start_pattern = re.compile(start_regex) if start_regex else None
    end_pattern = re.compile(end_regex) if end_regex else None

    placeholder_counter = 0
    placeholders_to_content = {}

    lines = text.splitlines(keepends=True)
    modified_lines = []
    inside_region = start_pattern is None
    current_placeholder_content = []

    for line in lines:
        if start_pattern and start_pattern.search(line[:-1]):
            # Start a new region (or stay inside currently open one), and accumulate content
            inside_region = True
            current_placeholder_content.append(line)
        elif end_pattern and end_pattern.search(line[:-1]):
            if inside_region:
                # End of the current region
                current_placeholder_content.append(line)
                placeholder_counter += 1
                placeholder = f"<@ PLACEHOLDER_{placeholder_counter} @>\n"
                placeholders_to_content[placeholder] = "".join(current_placeholder_content)
                modified_lines.append(placeholder)
                # Reset state
                inside_region = False
                current_placeholder_content = []
            else:
                # Lines outside the region
                modified_lines.append(line)
        else:
            if inside_region:
                # Accumulate lines within the region
                current_placeholder_content.append(line)
            else:
                # Lines outside the region
                modified_lines.append(line)

    # Handle any dangling open region
    if inside_region and not end_pattern:
        placeholder_counter += 1
        placeholder = f"<@ PLACEHOLDER_{placeholder_counter} @>\n"
        placeholders_to_content[placeholder] = "".join(current_placeholder_content)
        modified_lines.append(placeholder)

    # Join the modified lines into the final content
    modified_content = "".join(modified_lines)
    return modified_content, placeholders_to_content

def extract_and_replace_regions_between_v2(text, start_regex=None, end_regex=None):
    # This a pure-regex version. Note that it has really poor runtime
    # performance (several seconds per file!) if only end_regex is specified --> UNUSED!
    start_part = r"^.*" + start_regex if start_regex else ""
    body_part = r"(.|\n)*" + ("?" if end_regex else "")
    end_part = end_regex + r".*\n" if end_regex else ""
    regex = start_part + body_part + end_part

    placeholders = []
    placeholders_to_content = {}
    def replace_with_placeholder(match):
        placeholder = f"__PLACEHOLDER_{len(placeholders)}__"
        placeholders.append(placeholder)
        placeholders_to_content[placeholder] = match[0]
        return placeholder

    modified_content = re.sub(regex, replace_with_placeholder, text, flags=re.MULTILINE)
    return modified_content, placeholders_to_content

def verify_regions(original_content, modified_content, placeholders_to_content):
    restored_content = modified_content
    for placeholder in placeholders_to_content.keys():
        restored_content = restored_content.replace(placeholder, placeholders_to_content[placeholder])
    assert restored_content == original_content

def apply_command_to_content(file_path, content, context, file_type, task, custom_prompt, model, reply_format, region_seq=None, chunk_size=None, save_prompt=False, print_stats=False):
    if reply_format == "patch":
        prompt = create_prompt(file_path=file_path, content=content, context=context, task=task, custom_prompt=custom_prompt, file_type=file_type, reply_format=reply_format, region_seq=region_seq, save_prompt=save_prompt)
        reply_text = invoke_llm(prompt, model, print_stats=print_stats)
        if save_prompt:
            write_file(file_path+".reply", reply_text)

        blocks = parse_replace_blocks(reply_text)
        print(f"   {len(blocks)} blocks")
        modified_content = apply_replace_blocks(file_path, blocks)
        _logger.debug(f"Applied {len(blocks)} to {file_path}.")
        return modified_content

    elif reply_format == "updated-content":
        if chunk_size is None:
            chunk_size = get_llm_context_window(model)*2  - len(context) # assume average token length of 2 chars TODO also system prompt
        modified_content = ""
        content_parts = split_content(content, file_type, chunk_size)
        for i, content_part in enumerate(content_parts):
            if len(content_parts) > 1:
                print(f"   part {i + 1}/{len(content_parts)}")
            prompt = create_prompt(file_path=file_path, content=content_part, context=context, task=task, custom_prompt=custom_prompt, file_type=file_type, reply_format=reply_format, region_seq=region_seq, part_seq=i+1, num_parts=len(content_parts), save_prompt=save_prompt)
            reply_text = invoke_llm(prompt, model, print_stats=print_stats)
            modified_content += extract(reply_text, content_part)

        modified_content = discard_trailing_whitespace(modified_content)
        _logger.debug(f"Modified {file_path} successfully.")
        return modified_content

    else:
        raise ValueError(f"Unsupported reply format '{reply_format}'")

def apply_command_to_file(file_path, context_files, file_type, task, custom_prompt, model, reply_format, chunk_size=None,
                          with_context=True, start_at=None, end_at=None, blocks_containing=None, save_prompt=False, print_stats=False):
    if (start_at or end_at) and blocks_containing:
        raise ValueError("start-at/end-at and blocks-containing are mutually exclusive")
    if blocks_containing and file_type != "c++":
        raise ValueError("blocks-containing is only supported for C++ files")

    context_files = context_files or []
    if with_context:
        context_files += find_additional_context_files(file_path, file_type, task)
    if context_files:
        print("   context files: " + " ".join(context_files))
    context = read_and_quote_files(context_files)

    if not reply_format:
        reply_format = get_default_reply_format(file_type, task)

    content = read_file(file_path)

    if not start_at and not end_at and not blocks_containing:
        modified_content = apply_command_to_content(**dict_except(locals(), ["context_files", "placeholders_to_content", "with_context", "start_at", "end_at", "blocks_containing"]))
    else:
        modified_content, placeholders_to_content = \
            extract_and_replace_cpp_regions_containing(content, blocks_containing) if blocks_containing else \
            extract_and_replace_regions_between(content, start_at, end_at)
        verify_regions(content, modified_content, placeholders_to_content)
        if not placeholders_to_content:
            print("   no match, skipping")
        else:
            print(f"   {len(placeholders_to_content)} match(es)")
        # print("DEBUG:g", placeholders_to_content)
        region_seq = 0
        for placeholder, content_region in placeholders_to_content.items():
            modified_region = apply_command_to_content(content=content_region, **dict_except(locals(), ["content", "content_region", "modified_content", "modified_region", "placeholder", "context_files", "placeholders_to_content", "with_context", "start_at", "end_at", "blocks_containing"]))
            modified_content = modified_content.replace(placeholder, modified_region)
            region_seq +=1

    write_file(file_path, modified_content)

def apply_command_to_files(file_list, context_files, file_type, task, custom_prompt, model, reply_format,
                           chunk_size=None, start_at=None, end_at=None, blocks_containing=None,
                           with_context=True, save_prompt=False, print_stats=False):
    args = dict(locals())
    del args["file_list"]

    n = len(file_list)
    for i, file_path in enumerate(file_list):
        try:
            print(f"Processing file {i + 1}/{n} {file_path}")
            apply_command_to_file(file_path=file_path, **args)
        except Exception as e:
            print(f"-> Exception: {e}", file=sys.stderr)

def detect_file_type(paths):
    # collect files
    file_list = []
    for path in paths:
        if os.path.isdir(path):
            file_list.extend(collect_matching_file_paths(path))
        elif os.path.isfile(path):
            file_list.append(path)

    # count how many there are from each type
    from collections import Counter
    types_counts = Counter()
    for file_path in file_list:
        if file_path.endswith(".md"):
            type = "md"
        elif file_path.endswith(".rst"):
            type = "rst"
        elif file_path.endswith(".tex"):
            type = "tex"
        elif file_path.endswith(".ned"):
            type = "ned"
        elif file_path.endswith(".py"):
            type = "py"
        elif file_path.endswith(".cc") or file_path.endswith(".h"):
            type = "c++"
        else:
            type = "unknown"
        types_counts[type] += 1

    _logger.debug(f"Detected file types: {types_counts}")

    # choose the most common file type
    types_counts.pop("unknown", None)
    if not types_counts:
        raise ValueError("Cannot determine file type.")
    return types_counts.most_common(1)[0][0]

def resolve_file_list(paths, file_type, file_ext=None):
    file_extension_patterns = {
        "md":  r".*\.md$",
        "rst": r".*\.rst$",
        "tex": r".*\.tex$",
        "ned": r".*\.ned$",
        "py":  r".*\.py$",
        "c++": r".*\.(h|cc)$"
    }

    if file_ext:
        filename_regex = re.compile(rf".*\.{re.escape(file_ext)}$")
    else:
        if file_type not in file_extension_patterns:
            raise ValueError("Unsupported file type.")
        filename_regex = re.compile(file_extension_patterns[file_type])

    file_list = []
    for path in paths:
        if os.path.isdir(path):
            file_list.extend(collect_matching_file_paths(path, filename_regex))
        elif os.path.isfile(path) and filename_regex.match(path):
            file_list.append(path)

    # Sort the file list. We use custom sort to ensure that C++ headers are processed
    # before their .cc files, as this is more convenient for refactoring operations.
    def sort_key(file_name):
        return file_name.removesuffix(".h")
    return sorted(file_list, key=sort_key)

def process_files(paths, context_files, file_type, file_ext, task, custom_prompt, prompt_file,
                  model_name, reply_format, chunk_size=None, with_context=True,
                  start_at=None, end_at=None, blocks_containing=None, save_prompt=False, print_stats=False):
    if prompt_file:
        if custom_prompt:
            raise ValueError("Cannot specify both --prompt and --prompt-file")
        custom_prompt = read_file(prompt_file)

    if custom_prompt:
        task = None

    if not model_name:
        model_name = get_recommended_model(task, file_type)
    model = llm.get_model(model_name)
    model.key = ''

    if model_name.startswith("gpt-") and not os.getenv("OPENAI_API_KEY"):
        raise Exception(f"The OPENAI_API_KEY environment variable is not set -- it is required to use the '{model_name}' LLM model")

    if not reply_format:
        reply_format = get_default_reply_format(file_type, task)
    if not file_type:
        file_type = detect_file_type(paths)
        print("File type (autodetected): " + file_type)

    file_list = resolve_file_list(paths, file_type, file_ext)
    print("Files to process: " + " ".join(file_list))
    print("Task: " + (task or "custom prompt"))
    print("Using LLM: " + model_name)
    print("Reply format: " + reply_format)
    del prompt_file, model_name, file_ext, paths
    apply_command_to_files(**locals())

def main():
    parser = argparse.ArgumentParser(prog="llmtool", description="Process and improve specific types of files in a given directory or files.")
    parser.add_argument("paths", type=str, nargs='+', help="The directories or files to process.")
    parser.add_argument("--file-type", type=str, choices=["md", "rst", "tex", "ned", "py", "c++"], help="The type of files to process.")
    parser.add_argument("--file-ext", type=str, help="The extension of files to process. Takes precedence over --file-type.")
    parser.add_argument("--task", type=str, choices=["proofread", "improve", "eliminate-you-addressing", "neddoc"], default="improve", help="The task to perform on the files.")
    parser.add_argument("--prompt", type=str, dest="custom_prompt", help="Custom LLM prompt to use. Generic instructions and the content of context files will be added. Takes precedence over --task.")
    parser.add_argument("--prompt-file", type=str, help="Like --prompt, but allows the text to be provided in a file.")
    parser.add_argument("--reply-format", type=str, choices=["patch", "updated-content"], help="The format of the reply from the LLM model. The default depends on the file type and task. Patch is faster and cheaper due to being more concise, but it is also less reliable.")
    parser.add_argument("--model", type=str, dest="model_name", default=None, help="The name of the LLM model to use.")
    parser.add_argument("--chunk-size", type=int, default=None, help="The maximum number of characters to be sent to the LLM model at once.")
    parser.add_argument("--context-files", type=str, nargs='*', help="Files to the added to the prompt as context.")
    parser.add_argument("--without-context", dest="with_context", action='store_false', help="Whether to add discovered context files to the prompt. (This does not affect --context-files, they are always added.)")
    parser.add_argument("--start-at", type=str, help="Regular expression to match the first line of the region (or regions) to process. Lines outside matched regions are not submitted to the LLM. Example: To process initialize() functions in a C++ source, use --start-at='::initialize\\(' --end-at='^}'. To process constructor implementations in a C++ source, use --start-at='\\b(\\w+)::\\1' --end-at='^}'")
    parser.add_argument("--end-at", type=str, help="Regular expression to match the last line of the region (or regions) to process. Lines outside matched regions are not submitted to the LLM.")
    parser.add_argument("--blocks-containing", type=str, help="For C++ files only: Process source code regions (enclosing method definition, class declaration, etc.) containing the given regex pattern. Lines outside matched regions are not submitted to the LLM. Assumes that the C++ code is properly formatted, i.e. indentation conforms to the accepted norms.")
    parser.add_argument("--save-prompt", action='store_true', help="Save the LLM prompt for each input file as <filename>.prompt<N>.")
    args = parser.parse_args()
    try:
        process_files(**vars(args), print_stats=True)
    except KeyboardInterrupt:
        print("Interrupted", file=sys.stderr)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)

