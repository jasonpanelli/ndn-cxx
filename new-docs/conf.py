# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
from pathlib import Path

PROJECT_ROOT_DIR = Path(__file__).parents[1].resolve()

_docs_path = os.path.dirname(__file__)
_version_path = os.path.abspath(
    os.path.join(_docs_path, "..", "aiohttp", "__init__.py")
)
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'ndn-cxx Docs'
copyright = '2022, Sam, Emily, Jason'
author = 'Sam, Emily, Jason'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    # stdlib-party extensions:
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
    "sphinx.ext.viewcode",
    "sphinx_copybutton"
]

try:
    import sphinxcontrib.spelling  # noqa

    extensions.append("sphinxcontrib.spelling")
except ImportError:
    pass

intersphinx_mapping = {
    "python": ("http://docs.python.org/3", None),
    "multidict": ("https://multidict.readthedocs.io/en/stable/", None),
    "yarl": ("https://yarl.readthedocs.io/en/stable/", None),
    "aiosignal": ("https://aiosignal.readthedocs.io/en/stable/", None),
    "aiohttpjinja2": ("https://aiohttp-jinja2.readthedocs.io/en/stable/", None),
    "aiohttpremotes": ("https://aiohttp-remotes.readthedocs.io/en/stable/", None),
    "aiohttpsession": ("https://aiohttp-session.readthedocs.io/en/stable/", None),
    "aiohttpdemos": ("https://aiohttp-demos.readthedocs.io/en/latest/", None),
    "asynctest": ("https://asynctest.readthedocs.io/en/latest/", None),
}

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

source_suffix = ".rst"

master_doc = "index"

github_url = "https://github.com"
github_repo_org = "named-data"
github_repo_name = "ndn-cxx"
github_repo_slug = f"{github_repo_org}/{github_repo_name}"
github_repo_url = f"{github_url}/{github_repo_slug}"
github_sponsors_url = f"{github_url}/sponsors"

project = github_repo_name
copyright = f"{project} contributors"

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.


# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

exclude_patterns = ["_build"]

highlight_language = "python3"

html_theme = "sphinx_rtd_theme"

# html_theme_options = {
#     "description": "NDN C++ library with eXperimental eXtensions",
#     "canonical_url": "http://docs.ndn-cxx.org/en/stable/",
#     "github_user": github_repo_org,
#     "github_repo": github_repo_name,
#     "github_button": True,
#     "github_type": "star",
#     "github_banner": True,
#     "badges": [
#         {
#             "image": f"{github_repo_url}/actions/workflows/ci.yml/badge.svg",
#             "target": f"{github_repo_url}/actions/workflows/ci.yml",
#             "height": "20",
#             "alt": "Azure Pipelines CI status",
#         },
#         {
#             "image": f"https://github.com/named-data/ndn-cxx/actions/workflows/docs.yml/badge.svg",
#             "target": f"https://github.com/named-data/ndn-cxx/actions/workflows/docs.yml",
#             "height": "20",
#             "alt": "Docs",
#         },
#         {
#             "image": f"https://img.shields.io/badge/C%2B%2B-14-blue",
#             "target": f"https://img.shields.io/badge/C%2B%2B-14-blue",
#             "height": "20",
#             "alt": "Language",
#         },
#         {
#             "image": f"https://img.shields.io/github/v/tag/named-data/ndn-cxx?label=Latest%20version",
#             "target": f"https://img.shields.io/github/v/tag/named-data/ndn-cxx?label=Latest%20version",
#             "height": "20",
#             "alt": "Latest version",
#         },
#     ],
# }

html_css_files = [
    "css/logo-adjustments.css",
]

# Add any paths that contain custom themes here, relative to this directory.
# html_theme_path = [alabaster.get_path()]

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
# html_title = None

# A shorter title for the navigation bar.  Default is the same as html_title.
# html_short_title = None

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
html_logo = "ndn-plain.svg"

# The name of an image file (within the static path) to use as favicon of the
# docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
html_favicon = "ndn-plain.ico"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]

# Add any extra paths that contain custom files (such as robots.txt or
# .htaccess) here, relative to this directory. These files are copied
# directly to the root of the documentation.
# html_extra_path = []

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
# html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
# html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
html_sidebars = {
    "**": [
        "about.html",
        "navigation.html",
        "searchbox.html",
    ]
}

# Additional templates that should be rendered to pages, maps page names to
# template names.
# html_additional_pages = {}

# If false, no module index is generated.
# html_domain_indices = True

# If false, no index is generated.
# html_use_index = True

# If true, the index is split into individual pages for each letter.
# html_split_index = False

# If true, links to the reST sources are added to the pages.
# html_show_sourcelink = True

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
# html_show_sphinx = True

# If true, "(C) Copyright ..." is shown in the HTML footer. Default is True.
# html_show_copyright = True

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
# html_use_opensearch = ''

# This is the file name suffix for HTML files (e.g. ".xhtml").
# html_file_suffix = None

# Output file base name for HTML help builder.
htmlhelp_basename = f"{project}doc"


# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    # 'papersize': 'letterpaper',
    # The font size ('10pt', '11pt' or '12pt').
    # 'pointsize': '10pt',
    # Additional stuff for the LaTeX preamble.
    # 'preamble': '',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (
        "index",
        f"{project}.tex",
        f"{project} Documentation",
        f"{project} contributors",
        "manual",
    ),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
# latex_logo = None

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
# latex_use_parts = False

# If true, show page references after internal links.
# latex_show_pagerefs = False

# If true, show URL addresses after external links.
# latex_show_urls = False

# Documents to append as an appendix to all manuals.
# latex_appendices = []

# If false, no module index is generated.
# latex_domain_indices = True


# -- Options for manual page output ---------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [("index", project, f"{project} Documentation", [project], 1)]

# If true, show URL addresses after external links.
# man_show_urls = False


# -- Options for Texinfo output -------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (
        "index",
        project,
        f"{project} Documentation",
        "Aiohttp contributors",
        project,
        "One line description of project.",
        "Miscellaneous",
    ),
]